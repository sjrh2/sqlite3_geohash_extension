//sqlilte_geohash

#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1

#include <math.h>
#include <float.h>
#include <string.h>
#include <stdio.h>
#include "geohash.h"

/* BASE 32 encode table */
static char base32en[] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'b', 'c', 'd', 'e', 'f', 'g',
	'h', 'j', 'k', 'm', 'n', 'p', 'q', 'r',
	's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

#define BASE32DE_FIRST	'0'
#define BASE32DE_LAST	'z'
/* ASCII order for BASE 32 decode,from '0' to 'z', -1 in unused character */
static signed char base32de[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	10, 11, 12, 13, 14, 15, 16, -1, 17, 18,
	-1, 19, 20, -1, 21, 22, 23, 24, 25, 26,
	27, 28, 29, 30, 31
};

#define PRECISION	.00000000001	/* XXX not validated */

static int
max(int x, int y)
{
	if (x < y)
		return y;
	return x;
}

static double
pround(double x, int precision)
{
	double div = pow(10, precision);
	return round(x * div) / div;
}

/* Quick and Dirty Floating-Point to Fractional Precision */
static double
fprec(double x)
{
        int i;
        double int_part, frac_part;

        frac_part = modf(x, &int_part);

	/* check fractional is really close 0.0 or 1.0 */
	for (i = 0; fabs(frac_part - 0.0) > FLT_EPSILON && 
		    fabs(frac_part - 1.0) > FLT_EPSILON; i++) {
                frac_part *= 10;
                frac_part = modf(frac_part, &int_part);
        }
	return pow(10, -i);
}

int
geohash_encode(double latitude, double longitude, char *hash, size_t len)
{
	double lat[] = {-90.0, 90.0};
	double lon[] = {-180.0, 180.0};
	double mid;

	double precision;

	char mask[] = {16, 8, 4, 2, 1};

	int i, n;
	int idx;

	int even = 0;
	int right;

	/* check input latitude/longitude is ok or invalid */
	if (latitude < lat[0] || latitude > lat[1] ||
	    longitude < lon[0] || longitude > lon[1])
		return GEOHASH_INVALID;

	precision = fmin(fprec(latitude), fprec(longitude));
	precision = fmax(PRECISION, precision);

	/* save the last space for '\0' when len is not enough */
	len -= 1;
	for (i = 0; i < len; i++) {
		/* break when precision is enough for input latitude/longitude */
		if ((lat[1] - lat[0]) / 2.0 < precision && 
		    (lon[1] - lon[0]) / 2.0 < precision)
			break;

		for (n = idx = 0; n <= 4; n++) {
			if ((even = !even)) {
				mid = (lon[0] + lon[1]) / 2.0;
				right = (longitude > mid);
				if (right)
					idx |= mask[n];
				lon[!right] = mid;
			} else {
				mid = (lat[0] + lat[1]) / 2.0;
				right = (latitude > mid);
				if (right)
					idx |= mask[n];
				lat[!right] = mid;
			}
		}
		hash[i] = base32en[idx];
	}
	hash[i] = 0;
	return GEOHASH_OK;
}

int
geohash_decode(char *hash, double *latitude, double *longitude)
{
	size_t len = strlen(hash);

	double lat[] = {-90.0, 90.0};
	double lon[] = {-180.0, 180.0};
	double mid;

	double lat_err, lon_err;

	char mask[] = {16, 8, 4, 2, 1};

	int i, n;
	int idx = 0;

	int even = 0;

	int right;

	for (i = 0; i < len; i++) {
		if (hash[i] < BASE32DE_FIRST || hash[i] > BASE32DE_LAST ||
		    (idx = base32de[hash[i] - BASE32DE_FIRST]) == -1)
			return GEOHASH_INVALID;

		for (n = 0; n <= 4; n++) {
			if ((even = !even)) {
				mid = (lon[0] + lon[1]) / 2.0;
				right = (idx & mask[n]);
				lon[!right] = mid;
			} else {
				mid = (lat[0] + lat[1]) / 2.0;
				right = (idx & mask[n]);
				lat[!right] = mid;
			}
		}
	}
	lat_err = (lat[1] - lat[0]) / 2.0;
	*latitude = pround((lat[0] + lat[1]) / 2.0,
			max(1, (int)round(-log10(lat_err))) - 1);
	
	lon_err = (lon[1] - lon[0]) / 2.0;
	*longitude = pround((lon[0] + lon[1]) / 2.0, 
			max(1, (int)round(-log10(lon_err))) - 1);

	return GEOHASH_OK;
}

static void geohash_encode_api(sqlite3_context *context, int argc, sqlite3_value **argv)
{
	if (argc == 0 || argc == 1 || argc > 2 ) return;
	double lat = sqlite3_value_double(argv[0]);
	double lon = sqlite3_value_double(argv[1]);
	char geohash[64] = {0};
	if (geohash_encode(lat, lon, geohash, sizeof(geohash)) != GEOHASH_OK)
			printf("encode error\n");
	else
			sqlite3_result_text(context, geohash, -1, SQLITE_TRANSIENT);
}

static void geohash_decode_api(sqlite3_context *context, int argc, sqlite3_value **argv)
{
	if (argc == 0 || argc > 1) return;
	double lat, lon;
	const unsigned char *geoH_in;
	geoH_in = (const unsigned char*)sqlite3_value_text(argv[0]);
	//unsigned char geohash[64] = *sqlite3_value_text(argv[0]);
	char lat_lon[64];
	if (geohash_decode((char *)geoH_in, &lat, &lon) != GEOHASH_OK)
			printf("decode error\n");
	else
			sprintf(lat_lon, "lat: %.14f, lon: %.14f\n", lat, lon);
			sqlite3_result_text(context, lat_lon, -1, SQLITE_TRANSIENT);
			
}

int sqlite3_extension_init(
  sqlite3 *db, 
  char **pzErrMsg, 
  const sqlite3_api_routines *pApi
){
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  (void)pzErrMsg;  /* Unused parameter */
  rc = sqlite3_create_function(db, "geohash_encode", 2,
                   SQLITE_UTF8|SQLITE_INNOCUOUS|SQLITE_DETERMINISTIC,
                   0, geohash_encode_api, 0, 0);
  if( rc==SQLITE_OK ){
    rc = sqlite3_create_function(db, "geohash_decode", 1,
                   SQLITE_UTF8|SQLITE_INNOCUOUS|SQLITE_DETERMINISTIC,
                   0, geohash_decode_api, 0, 0);
  }
  return rc;
}


// optional system registry functionaly -- udf.
// int register_geohash_encode(sqlite3 *db)
// {
// 	return sqlite3_create_function(db, "geohash_encode", 2,
// 		SQLITE_UTF8, NULL, geohash_encode_api, NULL, NULL);
// }


// int register_geohash_decode(sqlite3 *db)
// {
// 	return sqlite3_create_function(db, "geohash_decode", 2,
// 		SQLITE_UTF8, NULL, geohash_decode_api, NULL, NULL);
// }
