/*
** 2025-03-24
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** SQL functions for geohash encoding, decoding and neighbor operayions.
**
**      ghash_encode(34.01, 127.01)      encode the lat, long into a geohash address
**
**      ghash-decode('34mnpqr')          decode the geohash into a lat and long coordinate pair
*/

#include <sqlite3.h>
#include <string.h>
#include <math.h>
#include "sqlite3_geohash.h"
#include "geohash.h"

void validate_Lat_Lon(sqlite3_context *context, double lat, double lon){
	if (a == SQLITE_NULL || b == SQLITE_NULL) {
    sqlite3_result_error(context, "Null types.", -1);
  }
  if (a != SQLITE_FLOAT || a != b) {
    sqlite3_result_error(context, "Incompatible types.", -1);
  }
} 

void geohash_encode(	sqlite3_context *context, int argc, sqlite3_value **argv){
	// first: check how geohash.h wants to get lat,lon pairs
	GeoHash::Point *p
	double lat, lon
	p = sqlite3_aggregate_context(context, sizeof(*p))
	lat = sqlite3_value_numeric_type(argv[0]);
  	lon = sqlite3_value_numeric_type(argv[1]);

}

void Geohash_decode( sqlite3_context *context, int argc, sqlite3_value **argv){

}

void geohash_decode_rectangle( sqlite3_context *context, int argc, sqlite3_value **argv){
  
}