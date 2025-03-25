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

#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <assert.h>
#include <string.h>

void setup_function(sqlite3 *db);

#ifdef _WIN32         // either it goes here or above the first line
__declspec(dllexport)
#endif
int sqlite3_geohash_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi );

void setup_functions(sqlite3 *db) {
  sqlite3_create_function(db, "gh_encode", 2, SQLITE_UTF8, NULL, geohash_encode, NULL, NULL);
  sqlite3_create_function(db, "gh_decode", 2, SQLITE_UTF8, NULL, geohash_decode, NULL, NULL); 
  sqlite3_create_function(db, "gh_decode_rectangle", 2, SQLITE_UTF8, NULL, geohash_decode, NULL, NULL);
}

// static void geohash_encode(
// 	sqlite3_context *context,
//   int argc,
//   sqlite3_value **argv){
// 	// first: check how geohash.h wants to get lat,lon pairs
// }

// static void geohash_decode_rectangle(
// 	sqlite3_context *context,
//   int argc,
//   sqlite3_value **argv){
// }

int sqlite3_geohash_init( sqlite3 *db, char **pzErrMsg,  const sqlite3_api_routines *pApi)
{
  // int rc = SQLITE_OK;
  // SQLITE_EXTENSION_INIT2(pApi);
  // (void)pzErrMsg;  /* Unused parameter */
  // rc = sqlite3_create_function(db, "ghash_encode", 2, SQLITE_UTF8, 0, geohash_encode, 0, 0);
  // if( rc==SQLITE_OK ){
  //   rc = sqlite3_create_function(db, "ghash_decode_rectangle", 1, SQLITE_UTF8, 0,
  //                              geohash_decode_rectangle, 0, 0);
  // }
  // ^^^ discard all this - keeping for reference in the mean time
  UNUSED(pzErrMsg);
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  setup_functions(db);
  return rc;
}