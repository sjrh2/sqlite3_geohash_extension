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

static void geohash_encode(	sqlite3_context *context, int argc, sqlite3_value **argv){

	// first: check how geohash.h wants to get lat,lon pairs
}

static void Geohash_decode( sqlite3_context *context, int argc, sqlite3_value **argv){

}

static void geohash_decode_rectangle( sqlite3_context *context, int argc, sqlite3_value **argv){
  
}