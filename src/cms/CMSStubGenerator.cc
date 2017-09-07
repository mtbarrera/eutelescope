/*
 * Created by Thomas Eichhorn
 *  (2017 DESY)
 *
 *  email:thomas.eichhorn@desy.de
 */


// ROOT includes:

// eutelescope includes ".h"
#include "CMSStubGenerator.h"
#include "EUTelRunHeaderImpl.h"
#include "EUTelEventImpl.h"
#include "EUTELESCOPE.h"
#include "EUTelGenericSparsePixel.h"
#include "EUTelSparseClusterImpl.h"
#include "EUTelGenericSparseClusterImpl.h"
#include "EUTelSimpleVirtualCluster.h"
#include "EUTelVirtualCluster.h"

// marlin includes ".h"
#include "marlin/Processor.h"
#include "marlin/Global.h"
#include "marlin/AIDAProcessor.h"

// lcio includes <.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/TrackerHitImpl.h>
#include <UTIL/CellIDDecoder.h>
#include <UTIL/CellIDEncoder.h>

// system includes <>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>
#include <iomanip>
#include <cstdio>

using namespace std;
using namespace lcio;
using namespace marlin;
using namespace gear;
using namespace IMPL;
using namespace eutelescope;


CMSStubGenerator::CMSStubGenerator ( ) : Processor ( "CMSStubGenerator" )
{
    // modify processor description
    _description =  "CMSStubGenerator merges the two CBC sensors' hits into stubs, based on the cluster positions. In modes 1 and 2, it can also drop either sensor's hits to have only one sensor remaining.";

    registerInputCollection ( LCIO::TRACKERHIT, "InputHitCollectionName", "Input hit collection name. Hits should be in global coordinates and pre-aligned", _inputHitCollectionName, std::string ( " " ) );

    registerOutputCollection ( LCIO::TRACKERHIT, "OutputHitCollectionName", "Output hit collection name", _outputHitCollectionName, std::string ( " " ) );

    registerProcessorParameter ( "DUTPlane1", "This is the first DUT sensorID.", _dutPlane1, int ( 6 ) );

    registerProcessorParameter ( "DUTPlane2", "This is the second DUT sensorID.", _dutPlane2, int ( 7 ) );

    registerProcessorParameter ( "KeepDUTHits", "Keep the DUT hits in mode 0 after creating subs or discard them?", _keepDUTHits, bool ( true ) );

    registerProcessorParameter ( "MaxResidual", "Maximum distance in channels for two clusters to constitute a stub", _maxResidual, float ( 0.0 ) );

    registerProcessorParameter ( "Mode", "0 to merge hits. 1 to drop hits from plane 1, 2 to drop hits from plane 2", _runMode, int ( 0 ) );

    registerProcessorParameter ( "OutputSensorID", "SensorID of the output stub.", _outputSensorID, int ( 8 ) );

}


void CMSStubGenerator::init ( )
{

    printParameters ( );

    bookHistos ( );

}


void CMSStubGenerator::processRunHeader ( LCRunHeader * rdr )
{
    std::unique_ptr < EUTelRunHeaderImpl > header = std::make_unique < EUTelRunHeaderImpl > ( rdr );
    header -> addProcessor ( type ( ) );

}


void CMSStubGenerator::processEvent ( LCEvent * event )
{

    EUTelEventImpl * evt = static_cast < EUTelEventImpl* > ( event );

    if ( evt -> getEventType ( ) == kEORE )
    {
	streamlog_out ( DEBUG4 ) << "EORE found: nothing else to do." << endl;
        return;
    }
    else if ( evt -> getEventType ( ) == kUNKNOWN )
    {
	streamlog_out ( WARNING2 ) << "Event number " << evt -> getEventNumber ( ) << " in run " << evt -> getRunNumber ( ) << " is of unknown type. Continue considering it as a normal data event." << endl;
    }

    LCCollectionVec * inputHitCollection = 0;
    LCCollectionVec * outputHitCollection = 0;

    try
    {
	inputHitCollection = static_cast < LCCollectionVec* > ( event -> getCollection ( _inputHitCollectionName ) );
    }
    catch ( DataNotAvailableException& e )
    {
	streamlog_out ( MESSAGE2 ) << "No input collection " << _inputHitCollectionName << " found in event " << event -> getEventNumber ( ) << " in run " << event -> getRunNumber ( ) << endl;
	return;
    }

    try
    {
	outputHitCollection = static_cast < LCCollectionVec* > ( event -> getCollection ( _outputHitCollectionName ) );
    }
    catch ( ... )
    {
	outputHitCollection = new LCCollectionVec ( LCIO::TRACKERHIT );
    }

    // prepare an encoder for the hit collection
    CellIDEncoder < TrackerHitImpl > outputCellIDEncoder ( EUTELESCOPE::HITENCODING, outputHitCollection );

    CellIDDecoder < TrackerHitImpl > inputCellIDDecoder ( inputHitCollection );

    vector < int > dutPlane1Hits;
    vector < int > dutPlane2Hits;

    for ( int iInputHits = 0; iInputHits < inputHitCollection -> getNumberOfElements ( ); iInputHits++ )
    {
	TrackerHitImpl * inputHit = dynamic_cast < TrackerHitImpl* > ( inputHitCollection -> getElementAt ( iInputHits ) );
        int sensorID = inputCellIDDecoder ( inputHit ) ["sensorID"];
	
	if ( _runMode == 0 )
	{
	    bool isDUTHit = false;

	    if ( sensorID == _dutPlane1 )
	    {
		dutPlane1Hits.push_back ( iInputHits );
		isDUTHit = true;
	    }
	    if ( sensorID == _dutPlane2 )
	    {
		dutPlane2Hits.push_back ( iInputHits );
		isDUTHit = true;
	    }

	    if ( !isDUTHit || _keepDUTHits )
	    {
		outputHitCollection -> push_back ( cloneHit ( inputHit ) );
	    }
	}
	else if ( _runMode == 1 )
	{
	    if ( sensorID != _dutPlane1 )
	    {
		outputHitCollection -> push_back ( cloneHit ( inputHit ) );
	    }
	}
	else if ( _runMode == 2 )
	{
	    if ( sensorID != _dutPlane2 )
	    {
		outputHitCollection -> push_back ( cloneHit ( inputHit ) );
	    }
	}
	else
	{
	    streamlog_out ( ERROR5 ) << "Illegal setting for Mode! Only 0, 1 or 2 is a valid setting!" << endl;
	    exit ( -1 );
	}
    }

    if ( _runMode == 0)
    {
	for ( unsigned int iHitPlane1 = 0; iHitPlane1 < dutPlane1Hits.size ( ); iHitPlane1++ )
	{
	    TrackerHitImpl * Hit1 = dynamic_cast < TrackerHitImpl* > ( inputHitCollection -> getElementAt ( dutPlane1Hits[iHitPlane1] ) );
	    float x1 = -1.0;
	    float y1 = -1.0;
	    float q1 = -1.0;
	    const double* pos1 = Hit1 -> getPosition ( );
	    TrackerDataImpl* clusterVector1 = static_cast < TrackerDataImpl* > ( Hit1 -> getRawHits ( ) [0] );
	    EUTelSimpleVirtualCluster * cluster1 = 0;
	    cluster1 = new EUTelSparseClusterImpl < EUTelGenericSparsePixel > ( clusterVector1 );
	    if ( cluster1 != 0 )
	    {
		cluster1 -> getCenterOfGravity ( x1, y1 );
		q1 = cluster1 -> getTotalCharge ( );
	    }

	    for ( unsigned int iHitPlane2 = 0; iHitPlane2 < dutPlane2Hits.size ( ); iHitPlane2++ )
	    {
		TrackerHitImpl * Hit2 = dynamic_cast < TrackerHitImpl* > ( inputHitCollection -> getElementAt ( dutPlane2Hits[iHitPlane2] ) );
		float x2 = -1.0;
		float y2 = -1.0;
		float q2 = -1.0;
		const double* pos2 = Hit2 -> getPosition ( );
		TrackerDataImpl* clusterVector2 = static_cast < TrackerDataImpl* > ( Hit2 -> getRawHits ( ) [0] );
		EUTelSimpleVirtualCluster * cluster2 = 0;
		cluster2 = new EUTelSparseClusterImpl < EUTelGenericSparsePixel > ( clusterVector2 );
		if ( cluster2 != 0 )
		{
		    cluster2 -> getCenterOfGravity ( x2, y2 );
		    q2 = cluster2 -> getTotalCharge ( );
		}
		streamlog_out ( DEBUG0 ) << " x1 " << x1 << " y1 " << y1 << " q1 " << q1 << " x2 " << x2 << " y2 " << y2 << " q2 " << q2 << " evt " << evt -> getRunNumber ( ) << endl;

		float dx = -1.0;
		float dy = -1.0;
		dx = fabs ( x1 - x2 );
		dy = fabs ( y1 - y2 );

		if ( dx < _maxResidual && dy < _maxResidual )
		{
		    double newPos[3];
		    newPos[0] = ( pos1[0] + pos2[0] ) / 2.0;
		    newPos[1] = ( pos1[1] + pos2[1] ) / 2.0;
		    newPos[2] = ( pos1[2] + pos2[2] ) / 2.0;

		    const double* hitpos = newPos;
		    CellIDEncoder < TrackerHitImpl > idHitEncoder ( EUTELESCOPE::HITENCODING, outputHitCollection );
		    TrackerHitImpl* hit = new TrackerHitImpl;
		    hit -> setPosition ( &hitpos[0] );
		    float cov[TRKHITNCOVMATRIX] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
		    hit -> setCovMatrix ( cov );
		    hit -> setType ( kEUTelGenericSparseClusterImpl );
		    // assume all times are equal
		    hit -> setTime ( Hit1 -> getTime ( ) );

		    LCObjectVec clusterVec;
		    clusterVec.push_back ( clusterVector1 );
		    clusterVec.push_back ( clusterVector2 );

		    hit -> rawHits ( ) = clusterVec;

		    idHitEncoder["sensorID"] =  _outputSensorID ;
		    idHitEncoder["properties"] = 0;

		    idHitEncoder.setCellID( hit );
		    outputHitCollection->push_back( hit );
		}
	    }
	}
    }

    try
    {
	event -> getCollection ( _outputHitCollectionName ) ;
    }
    catch ( ... )
    {
	event -> addCollection ( outputHitCollection, _outputHitCollectionName );
    }
}


TrackerHitImpl* CMSStubGenerator::cloneHit ( TrackerHitImpl *inputHit )
{
    TrackerHitImpl * newHit = new TrackerHitImpl;

    // copy hit position
    const double* hitPos = inputHit -> getPosition ( );
    newHit -> setPosition ( &hitPos[0] );

    // copy cov. matrix
    newHit -> setCovMatrix ( inputHit -> getCovMatrix ( ) );

    // copy type
    newHit -> setType ( inputHit -> getType ( ) );

    // copy rawhits
    LCObjectVec clusterVec = inputHit -> getRawHits ( );
    newHit -> rawHits ( ) = clusterVec;

    // copy cell IDs
    newHit -> setCellID0 ( inputHit -> getCellID0 ( ) );
    newHit -> setCellID1 ( inputHit -> getCellID1 ( ) );

    // copy EDep
    newHit -> setEDep ( inputHit -> getEDep ( ) );

    // copy EDepError
    newHit -> setEDepError ( inputHit -> getEDepError ( ) );

    // copy Time
    newHit -> setTime ( inputHit -> getTime ( ) );

    // copy Quality
    newHit -> setQuality ( inputHit -> getQuality ( ) );

    return newHit;
}


void CMSStubGenerator::end ( )
{

}

void CMSStubGenerator::bookHistos( )
{

}