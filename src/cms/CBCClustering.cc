/*
 * Created by Thomas Eichhorn
 *  (2017 DESY)
 *
 *  email:thomas.eichhorn@desy.de
 */

// marlin includes ".h"
#include "marlin/Processor.h"
#include "marlin/Exceptions.h"
#include "marlin/Global.h"

#if defined(USE_AIDA) || defined(MARLIN_USE_AIDA)
// aida includes <.h>
#include <marlin/AIDAProcessor.h>
#include <AIDA/ITree.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/IHistogram1D.h>
#endif

// lcio includes <.h>
#include <lcio.h>
#include <Exceptions.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/LCEventImpl.h>
#include <IMPL/LCGenericObjectImpl.h>
#include <IMPL/LCRunHeaderImpl.h>
#include <IMPL/TrackerPulseImpl.h>
#include <IMPL/TrackerRawDataImpl.h>
#include <IMPL/TrackerDataImpl.h>
#include <IO/LCWriter.h>
#include <IO/LCReader.h>
#include <UTIL/CellIDDecoder.h>
#include <UTIL/CellIDEncoder.h>
#include <UTIL/LCTime.h>
#include <UTIL/LCTOOLS.h>

// system includes <>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <memory>
#include <glob.h>
#include <vector>
#include <set>
#include <map>

// eutelescope includes ""
#include "EUTELESCOPE.h"
#include "EUTelGenericSparsePixel.h"
#include "EUTelSparseClusterImpl.h"
#include "EUTelGenericSparseClusterImpl.h"
#include "EUTelSimpleVirtualCluster.h"
#include "EUTelGeometricClusterImpl.h"
#include "EUTelFFClusterImpl.h"
#include "EUTelDFFClusterImpl.h"
#include "EUTelBrickedClusterImpl.h"
#include "anyoption.h"
#include "EUTELESCOPE.h"
#include "EUTelEventImpl.h"
#include "EUTelRunHeaderImpl.h"

#include "CBCClustering.h"


using namespace std;
using namespace lcio;
using namespace marlin;
using namespace IMPL;
using namespace eutelescope;


CBCClustering::CBCClustering ( ) : Processor ( "CBCClustering" ),
_aidaHistoMap ( )
{

    _description = "CBCClustering clusters the CBC data stream.";

    registerProcessorParameter ( "CBCInputCollectionName", "The name of the CBC collection we want to read", _cbcInputCollectionName, string ( "cbc_input" ) );

    registerProcessorParameter ( "CBCDataOutputCollectionName", "The name of the CBC data collection we want to write", _cbcDataOutputCollectionName, string ( "cbc_data_output" ) );

    registerProcessorParameter ( "CBCPulseOutputCollectionName", "The name of the CBC pulse collection we want to write", _cbcPulseOutputCollectionName, string ( "cbc_pulse_output" ) );

    registerProcessorParameter ( "NonSensitiveAxis", "The unsensitive axis of the CBC", _nonsensitiveaxis, string ( "x" ) );

    registerProcessorParameter ( "OutputSensorID", "The sensor id to write", _outputSensorID, int ( 6 ) );

}


void CBCClustering::init ( )
{
    streamlog_out ( MESSAGE4 ) << "Running init" << endl;
    printParameters ( );

    if ( _nonsensitiveaxis == "x" || _nonsensitiveaxis == "X" )
    {
	_nonsensitiveaxis = "x";
	streamlog_out ( MESSAGE4 ) << "Non-sensitive axis is x!" << endl;
    }
    else if ( _nonsensitiveaxis == "y" || _nonsensitiveaxis == "Y" )
    {
	_nonsensitiveaxis = "y";
	streamlog_out ( MESSAGE4 ) << "Non-sensitive axis is y!" << endl;
    }
    else
    {
	streamlog_out ( ERROR5 ) << "Illegal setting for NonSensitiveAxis! Valid coordinates are x and y!" << endl;
	exit ( -1 );
    }

}


void CBCClustering::processRunHeader ( LCRunHeader * rdr )
{
    streamlog_out ( MESSAGE4 ) << "Running processRunHeader" << endl;

    auto arunHeader = std::make_unique < EUTelRunHeaderImpl > ( rdr );
    arunHeader -> addProcessor ( type ( ) );

    bookHistos ( );

}


void CBCClustering::processEvent ( LCEvent * anEvent )
{

    if ( anEvent -> getEventNumber ( ) % 1000 == 0 )
    {
	streamlog_out ( MESSAGE4 ) << "Looping events " << anEvent -> getEventNumber ( ) << endl;
    }

	// the collection we read
	LCCollectionVec * inputCollectionVec;

	// the collection we output
	LCCollectionVec * clusterCollection;

	// the sparse collection we output
	LCCollectionVec * sparseClusterCollectionVec = NULL;
	sparseClusterCollectionVec =  new LCCollectionVec ( LCIO::TRACKERDATA );

	try
	{
	    clusterCollection = dynamic_cast < LCCollectionVec * > ( anEvent -> getCollection ( _cbcDataOutputCollectionName ) );
	}
	catch ( lcio::DataNotAvailableException& e )
	{
	    clusterCollection = new LCCollectionVec(LCIO::TRACKERPULSE);
	}

	// find the seed clusters on our data
	try
        {
		// give the collection vec its data
		inputCollectionVec = dynamic_cast < LCCollectionVec * > ( anEvent -> getCollection ( _cbcInputCollectionName ) );

		// loop over collection sizes, just in case
		int noOfDetector = inputCollectionVec -> getNumberOfElements ( );
                for ( int i = 0; i < noOfDetector; ++i ) 
                {
		    TrackerDataImpl * trkdata = dynamic_cast < TrackerDataImpl * > ( inputCollectionVec -> getElementAt ( i ) );

		    FloatVec datavec;
		    datavec = trkdata -> getChargeValues ( );

		    std::vector < int > clusterNumber;
		    clusterNumber.assign ( datavec.size ( ), 0 );
		    int nClusters = 0;
	
		    for ( size_t ichan = 0; ichan < datavec.size ( ); ichan++ )
		    {
			int cluleft = 0;
			int cluright = 0;
			int clusize = 0;
			if ( datavec[ichan] * 1.0 > 0 )
			{
			    streamlog_out ( DEBUG0 ) << "Chan " << ichan << " over threshold" << endl;
			    cluleft = ichan;
			    cluright = ichan;
			    bool search = true;
			    nClusters++;
			    clusize++;
			    dynamic_cast < AIDA::IHistogram1D* > ( _aidaHistoMap["Hitmap_" + to_string ( _outputSensorID ) ] ) -> fill ( ichan );
			    while ( search == true )
			    {
				if ( ( ichan + 1 ) < ( datavec.size ( ) - 1 ) )
				{
				    ichan++;
				    if ( datavec[ichan] > 0 )
				    {
					cluright = ichan;
					clusize++;
					dynamic_cast < AIDA::IHistogram1D* > ( _aidaHistoMap["Hitmap_" + to_string ( _outputSensorID ) ] ) -> fill ( ichan );
					streamlog_out ( DEBUG1 ) << "Chan " << ichan << " added to cluster, size now " << clusize << endl;
				    }
				    else
				    {
					search = false;
				    }
				}
				else
				{
				    search = false;
				}
			    }
			    for ( int k = cluleft; k <= cluright; k++ )
			    {
				clusterNumber.at ( k ) = nClusters;
			    }
			}
		    }

		    // now output the clusters we have found
		    CellIDEncoder < TrackerPulseImpl > zsDataEncoder ( eutelescope::EUTELESCOPE::PULSEDEFAULTENCODING, clusterCollection );
		    CellIDEncoder < TrackerDataImpl > idClusterEncoder ( eutelescope::EUTELESCOPE::ZSCLUSTERDEFAULTENCODING, sparseClusterCollectionVec );

		    // decode datavec into pixelvec, each chan goes into a pixel
		    std::vector < eutelescope::EUTelGenericSparsePixel* > PixelVec;
		    eutelescope::EUTelGenericSparsePixel Pixel;
		    std::unique_ptr < EUTelTrackerDataInterfacerImpl < EUTelGenericSparsePixel > > pixelData = std::make_unique < EUTelTrackerDataInterfacerImpl < EUTelGenericSparsePixel > > ( trkdata );
		    for ( unsigned int iPixel = 0; iPixel < datavec.size ( ); iPixel++ )
		    {
			pixelData -> getSparsePixelAt ( iPixel, &Pixel );
			if ( _nonsensitiveaxis == "x" )
			{
			    Pixel.setXCoord ( 0.0 );
			    Pixel.setYCoord ( iPixel );
			}
			else
			{
			    Pixel.setXCoord ( iPixel );
			    Pixel.setYCoord ( 0.0 );
			}
			Pixel.setSignal ( datavec[iPixel] * 1.0 );
			PixelVec.push_back (new EUTelGenericSparsePixel ( Pixel ) );
			streamlog_out ( DEBUG0 ) << "Decoded pixel with coordinates: ( " << Pixel.getXCoord ( ) << " , " << Pixel.getYCoord ( ) << " ) and charge: " << datavec[iPixel] << " !" << endl;
			streamlog_out ( DEBUG0 ) << "Input was: " << PixelVec.at ( iPixel ) << endl;
		    }

		    std::vector < int > ::iterator it;
		    for ( it = clusterNumber.begin ( ); it != clusterNumber.end ( ); ++it )
		    {
			lcio::TrackerPulseImpl * pulseFrame = new lcio::TrackerPulseImpl ( );
			lcio::TrackerDataImpl * clusterFrame = new lcio::TrackerDataImpl ( );
			eutelescope::EUTelSparseClusterImpl < eutelescope::EUTelGenericSparsePixel > *pixelCluster = new eutelescope::EUTelSparseClusterImpl < eutelescope::EUTelGenericSparsePixel > ( clusterFrame );
			
			for ( size_t j = 0; j < datavec.size ( ); j++ )
			{
			    if ( clusterNumber.at ( j ) == *it ) 
			    {
				if ( clusterNumber.at ( j ) > 0 )
				{
				    // put only these pixels in that ClusterCollection that belong to that cluster
				    pixelCluster -> addSparsePixel ( PixelVec.at ( j ) );
				    streamlog_out ( DEBUG1 ) << "Evt " << anEvent -> getEventNumber ( )<<" Adding channel " << j << " to cluster " << clusterNumber.at ( j ) << endl;
				    if ( it != clusterNumber.end ( ) - 1 )
				    {
					++it;
				    }
				}
			    }
			}

			// now we have pixelClusters with the corresponding pixels (which have the xy and q info) in them
			// this if stops making the "zero" cluster with all strips not in a cluster...
			unsigned int clumin = 1;
			unsigned int clumax = 99;
			if ( ( pixelCluster -> size ( ) >= clumin ) && ( pixelCluster -> size ( ) <= clumax ) && ( pixelCluster -> getTotalCharge ( ) >= 1 ) ) 
			{

			    // make a frame of each cluster and give it position, charge, etc. Then push back into clusterCollection and sparseClusterCollectionVec.
			    float x, y = 0;
			    int xsize, ysize = 0;
			    float charge = 0.0;
			    charge = pixelCluster -> getTotalCharge ( );
			    pulseFrame -> setCharge ( charge );
			    pixelCluster -> getCenterOfGravity ( x, y );
			    pixelCluster -> getClusterSize ( xsize, ysize );

			    streamlog_out( DEBUG1 ) << "Cluster: " << *it << ", Q: " << charge << " , x: " << x << " , y: " << y << " , dx: " << xsize << " , dy: " << ysize << " in event: " << anEvent -> getEventNumber ( ) << endl;

			    dynamic_cast < AIDA::IHistogram1D* > ( _aidaHistoMap["ClusterCharge_" + to_string ( _outputSensorID ) ] ) -> fill ( charge );
			    if ( _nonsensitiveaxis == "x" )
			    {
				dynamic_cast < AIDA::IHistogram1D* > ( _aidaHistoMap["ClusterSize_" + to_string ( _outputSensorID ) ] ) -> fill ( ysize );
			    }
			    if ( _nonsensitiveaxis == "y" )
			    {
				dynamic_cast < AIDA::IHistogram1D* > ( _aidaHistoMap["ClusterSize_" + to_string ( _outputSensorID ) ] ) -> fill ( xsize );
			    }

			    zsDataEncoder["sensorID"] = _outputSensorID;
			    zsDataEncoder["xSeed"] = static_cast < long > ( x );
			    zsDataEncoder["ySeed"] = static_cast < long > ( y );
			    zsDataEncoder["xCluSize"] = xsize;
			    zsDataEncoder["yCluSize"] = ysize;
			    zsDataEncoder["type"] = static_cast < int > ( kEUTelSparseClusterImpl );
			    zsDataEncoder["quality"] = static_cast < int > ( 0 );
			    zsDataEncoder.setCellID ( pulseFrame );
			    pulseFrame -> setTrackerData ( clusterFrame );
			    clusterCollection -> push_back ( pulseFrame );

			    idClusterEncoder["sensorID"] = _outputSensorID;
			    idClusterEncoder["sparsePixelType"] = static_cast < int > ( 2 );
			    idClusterEncoder["quality"] = static_cast < int > ( 0 );
			    idClusterEncoder.setCellID ( clusterFrame );
			    sparseClusterCollectionVec -> push_back ( clusterFrame );
			}
			else // clean up
			{
			    delete pulseFrame;
			    delete clusterFrame;
			}

			delete pixelCluster;

		    } // done clusterNumber iteration

		// we don't need the decoding any more, cleanup
		for ( std::vector < eutelescope::EUTelGenericSparsePixel* > ::iterator pixit = PixelVec.begin ( ); pixit < PixelVec.end ( ); pixit++ )
		{
		    delete *pixit;
		}
	    }
	}
	catch ( lcio::DataNotAvailableException )
	{

	}

	anEvent->addCollection( sparseClusterCollectionVec, _cbcDataOutputCollectionName );
	anEvent->addCollection( clusterCollection, _cbcPulseOutputCollectionName );

}


void CBCClustering::check ( LCEvent * /* evt */ )
{

}


void CBCClustering::end ( )
{
    streamlog_out ( MESSAGE4 ) << "Successfully finished!" << endl;

}


void CBCClustering::fillHistos ( )
{

}


void CBCClustering::bookHistos ( )
{

    string basePath = "Clustering_" + to_string ( _outputSensorID );
    AIDAProcessor::tree ( this ) -> mkdir ( basePath.c_str ( ) );
    basePath.append ( "/" );

    AIDA::IHistogram1D * csizeHist = AIDAProcessor::histogramFactory ( this ) -> createHistogram1D ( ( basePath + "ClusterSize_" + to_string ( _outputSensorID ) ).c_str ( ), 10, -0.5, 9.5 );
    _aidaHistoMap.insert ( make_pair ( "ClusterSize_" + to_string ( _outputSensorID ), csizeHist ) );
    csizeHist -> setTitle ( "Cluster Size;Cluster Size;Entries" );

    AIDA::IHistogram1D * cchargeHist = AIDAProcessor::histogramFactory ( this ) -> createHistogram1D ( ( basePath + "ClusterCharge_" + to_string ( _outputSensorID ) ).c_str ( ), 100, -0.5, 99.5 );
    _aidaHistoMap.insert ( make_pair ( "ClusterCharge_" + to_string ( _outputSensorID ), cchargeHist ) );
    cchargeHist -> setTitle ( "Cluster Charge;Cluster Charge;Entries" );

    AIDA::IHistogram1D * hitmapHist = AIDAProcessor::histogramFactory ( this ) -> createHistogram1D ( ( basePath + "Hitmap_" + to_string ( _outputSensorID ) ).c_str ( ), 1017, -0.5, 1016.5 );
    _aidaHistoMap.insert ( make_pair ( "Hitmap_" + to_string ( _outputSensorID ), hitmapHist ) );
    hitmapHist -> setTitle ( "Hitmap;Channel;Entries" );

}
