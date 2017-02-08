// -*- mode: c++; mode: auto-fill; mode: flyspell-prog; -*-
// Authors:
// Philipp Roloff, DESY <mailto:philipp.roloff@desy.de>
// Joerg Behr, Hamburg Uni/DESY <joerg.behr@desy.de>
// Slava Libov, DESY <mailto:vladyslav.libov@desy.de>
// Igor Rubinskiy, DESY <mailto:igorrubinsky@gmail.com>
// Daniel Pitzl, DESY <mailto:daniel.pitzl@desy.de>
// Thomas Eichhorn, DESY <mailto:thomas.eichhorn@desy.de>
//
/*
 *   This source code is part of the Eutelescope package of Marlin.
 *   You are free to use this source files for your own development as
 *   long as it stays in a public research context. You are not
 *   allowed to use it for commercial purpose. You must put this
 *   header with author names in all development based on this file.
 *
 */

// built only if GEAR and MARLINUTIL are used
#if defined(USE_GEAR) && defined(USE_MARLINUTIL)

// eutelescope includes ".h"
#include "EUTelMilleGBL.h"
#include "EUTelRunHeaderImpl.h"
#include "EUTelEventImpl.h"
#include "EUTELESCOPE.h"
#include "EUTelVirtualCluster.h"
#include "EUTelFFClusterImpl.h"
#include "EUTelDFFClusterImpl.h"
#include "EUTelBrickedClusterImpl.h"
#include "EUTelSparseClusterImpl.h"
#include "EUTelExceptions.h"
#include "EUTelPStream.h" // process streams redi::ipstream
#include "EUTelAlignmentConstant.h"
#include "EUTelGeometryTelescopeGeoDescription.h"

// GBL includes ".h"
#include "include/GblTrajectory.h"
#include "include/MilleBinary.h"

// marlin includes ".h"
#include "marlin/Processor.h"
#include "marlin/Global.h"
#include "marlin/Exceptions.h"
#include "marlin/AIDAProcessor.h"

// marlin util includes ".h"
#include "mille/Mille.h"

// gear includes <.h>
#include <gear/GearMgr.h>
#include <gear/SiPlanesParameters.h>
#include "gear/BField.h"

// aida includes <.h>
#if defined(USE_AIDA) || defined(MARLIN_USE_AIDA)
#include <marlin/AIDAProcessor.h>
#include <AIDA/IHistogramFactory.h>
#include <AIDA/IHistogram1D.h>
#include <AIDA/IHistogram2D.h>
#include <AIDA/IProfile2D.h>
#include <AIDA/ITree.h>
#include <AIDA/IAxis.h>
#endif

// lcio includes <.h>
#include <IO/LCWriter.h>
#include <UTIL/LCTime.h>
#include <EVENT/LCCollection.h>
#include <EVENT/LCEvent.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/TrackerHitImpl.h>
#include <IMPL/TrackImpl.h>
#include <IMPL/LCFlagImpl.h>
#include <Exceptions.h>
#include <UTIL/CellIDEncoder.h>
#include <IMPL/TrackerRawDataImpl.h>
#include <IMPL/TrackerDataImpl.h>
#include <IMPL/TrackerPulseImpl.h>
#include <IMPL/LCCollectionVec.h>

// ROOT includes
#if defined(USE_ROOT) || defined(MARLIN_USE_ROOT)
#include <TRandom.h>
//#include <TMinuit.h>
#include <TSystem.h>
#include <TMath.h>
#include <TVectorD.h>
#include <TMatrixD.h>
#include <TVector3.h>
#include <TRotation.h>
#include <TF1.h>
#include <TProfile.h>
#endif

// system includes <>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <memory>
#include <cmath>
#include <iostream>
#include <fstream>
#include <stdio.h>

#define PI 3.14159265

using namespace std;
using namespace lcio;
using namespace marlin;
using namespace eutelescope;

// definition of static members mainly used to name histograms
#if defined(USE_AIDA) || defined(MARLIN_USE_AIDA)

AIDA::IHistogram1D * dx01Hist;
AIDA::IHistogram1D * dy01Hist;
AIDA::IHistogram1D * dx02Hist;
AIDA::IHistogram1D * dy02Hist;
AIDA::IHistogram1D * dx03Hist;
AIDA::IHistogram1D * dy03Hist;
AIDA::IHistogram1D * dx04Hist;
AIDA::IHistogram1D * dy04Hist;
AIDA::IHistogram1D * dx05Hist;
AIDA::IHistogram1D * dy05Hist;
AIDA::IHistogram1D * dx06Hist;
AIDA::IHistogram1D * dy06Hist;
AIDA::IHistogram1D * dx07Hist;
AIDA::IHistogram1D * dy07Hist;

AIDA::IHistogram1D * dx12Hist;
AIDA::IHistogram1D * dy12Hist;
AIDA::IHistogram1D * dx13Hist;
AIDA::IHistogram1D * dy13Hist;
AIDA::IHistogram1D * dx14Hist;
AIDA::IHistogram1D * dy14Hist;
AIDA::IHistogram1D * dx15Hist;
AIDA::IHistogram1D * dy15Hist;
AIDA::IHistogram1D * dx16Hist;
AIDA::IHistogram1D * dy16Hist;
AIDA::IHistogram1D * dx17Hist;
AIDA::IHistogram1D * dy17Hist;

AIDA::IHistogram1D * dx23Hist;
AIDA::IHistogram1D * dy23Hist;
AIDA::IHistogram1D * dx24Hist;
AIDA::IHistogram1D * dy24Hist;
AIDA::IHistogram1D * dx25Hist;
AIDA::IHistogram1D * dy25Hist;
AIDA::IHistogram1D * dx26Hist;
AIDA::IHistogram1D * dy26Hist;
AIDA::IHistogram1D * dx27Hist;
AIDA::IHistogram1D * dy27Hist;

AIDA::IHistogram1D * dx34Hist;
AIDA::IHistogram1D * dy34Hist;
AIDA::IHistogram1D * dx35Hist;
AIDA::IHistogram1D * dy35Hist;
AIDA::IHistogram1D * dx36Hist;
AIDA::IHistogram1D * dy36Hist;
AIDA::IHistogram1D * dx37Hist;
AIDA::IHistogram1D * dy37Hist;

AIDA::IHistogram1D * dx45Hist;
AIDA::IHistogram1D * dy45Hist;
AIDA::IHistogram1D * dx46Hist;
AIDA::IHistogram1D * dy46Hist;
AIDA::IHistogram1D * dx47Hist;
AIDA::IHistogram1D * dy47Hist;

AIDA::IHistogram1D * dx56Hist;
AIDA::IHistogram1D * dy56Hist;
AIDA::IHistogram1D * dx57Hist;
AIDA::IHistogram1D * dy57Hist;

AIDA::IHistogram1D * dx67Hist;
AIDA::IHistogram1D * dy67Hist;

AIDA::IHistogram2D * dx01dtHist;
AIDA::IHistogram2D * dy01dtHist;
AIDA::IHistogram2D * dx12dtHist;
AIDA::IHistogram2D * dy12dtHist;
AIDA::IHistogram2D * dx23dtHist;
AIDA::IHistogram2D * dy23dtHist;
AIDA::IHistogram2D * dx34dtHist;
AIDA::IHistogram2D * dy34dtHist;
AIDA::IHistogram2D * dx45dtHist;
AIDA::IHistogram2D * dy45dtHist;
AIDA::IHistogram2D * dx26dtHist;
AIDA::IHistogram2D * dy26dtHist;
AIDA::IHistogram2D * dx57dtHist;
AIDA::IHistogram2D * dy57dtHist;

AIDA::IHistogram1D * trirxHist;
AIDA::IHistogram1D * triryHist;
AIDA::IHistogram1D * ntriHist;

AIDA::IHistogram1D * trirxdutHist;
AIDA::IHistogram1D * trirydutHist;

AIDA::IHistogram1D * drirxHist;
AIDA::IHistogram1D * driryHist;
AIDA::IHistogram1D * ndriHist;

AIDA::IHistogram1D * drirxrefHist;
AIDA::IHistogram1D * driryrefHist;

AIDA::IHistogram1D * sixrxHist;
AIDA::IHistogram1D * sixryHist;
AIDA::IHistogram1D * sixkxHist;
AIDA::IHistogram1D * sixkyHist;

AIDA::IHistogram1D * selxtHist;
AIDA::IHistogram1D * selytHist;
AIDA::IHistogram1D * selaxtHist;
AIDA::IHistogram1D * selaytHist;
AIDA::IHistogram1D * selxdHist;
AIDA::IHistogram1D * selydHist;
AIDA::IHistogram1D * selaxdHist;
AIDA::IHistogram1D * selaydHist;
AIDA::IHistogram1D * selrxHist;
AIDA::IHistogram1D * selryHist;
AIDA::IHistogram1D * selkxHist;
AIDA::IHistogram1D * selkyHist;

AIDA::IHistogram1D * selrx0Hist;
AIDA::IHistogram1D * selry0Hist;
AIDA::IHistogram1D * selrx1Hist;
AIDA::IHistogram1D * selry1Hist;
AIDA::IHistogram1D * selrx2Hist;
AIDA::IHistogram1D * selry2Hist;
AIDA::IHistogram1D * selrx3Hist;
AIDA::IHistogram1D * selry3Hist;
AIDA::IHistogram1D * selrx4Hist;
AIDA::IHistogram1D * selry4Hist;
AIDA::IHistogram1D * selrx5Hist;
AIDA::IHistogram1D * selry5Hist;
AIDA::IHistogram1D * selrx6Hist;
AIDA::IHistogram1D * selry6Hist;
AIDA::IHistogram1D * selrx7Hist;
AIDA::IHistogram1D * selry7Hist;

AIDA::IHistogram1D * gblndfHist;
AIDA::IHistogram1D * gblchi2Hist;
AIDA::IHistogram1D * gblchi2ndfHist;
AIDA::IHistogram1D * gblprbHist;

AIDA::IHistogram1D * badxHist;
AIDA::IHistogram1D * badyHist;
AIDA::IHistogram1D * badaxHist;
AIDA::IHistogram1D * badayHist;
AIDA::IHistogram1D * baddxHist;
AIDA::IHistogram1D * baddyHist;
AIDA::IHistogram1D * badkxHist;
AIDA::IHistogram1D * badkyHist;

AIDA::IHistogram1D * baddx0Hist;
AIDA::IHistogram1D * baddy0Hist;
AIDA::IHistogram1D * baddx1Hist;
AIDA::IHistogram1D * baddy1Hist;
AIDA::IHistogram1D * baddx2Hist;
AIDA::IHistogram1D * baddy2Hist;
AIDA::IHistogram1D * baddx3Hist;
AIDA::IHistogram1D * baddy3Hist;
AIDA::IHistogram1D * baddx4Hist;
AIDA::IHistogram1D * baddy4Hist;
AIDA::IHistogram1D * baddx5Hist;
AIDA::IHistogram1D * baddy5Hist;
AIDA::IHistogram1D * baddx6Hist;
AIDA::IHistogram1D * baddy6Hist;
AIDA::IHistogram1D * baddx7Hist;
AIDA::IHistogram1D * baddy7Hist;

AIDA::IHistogram1D * goodx0Hist;
AIDA::IHistogram1D * goody0Hist;
AIDA::IHistogram1D * goodx1Hist;
AIDA::IHistogram1D * goody1Hist;
AIDA::IHistogram1D * goodx2Hist;
AIDA::IHistogram1D * goody2Hist;
AIDA::IHistogram1D * goodx3Hist;
AIDA::IHistogram1D * goody3Hist;
AIDA::IHistogram1D * goodx4Hist;
AIDA::IHistogram1D * goody4Hist;
AIDA::IHistogram1D * goodx5Hist;
AIDA::IHistogram1D * goody5Hist;
AIDA::IHistogram1D * goodx6Hist;
AIDA::IHistogram1D * goody6Hist;
AIDA::IHistogram1D * goodx7Hist;
AIDA::IHistogram1D * goody7Hist;

AIDA::IHistogram2D * dutrxxHist;
AIDA::IHistogram2D * dutrxyHist;
AIDA::IHistogram2D * dutrxzHist;

AIDA::IHistogram2D * dutryxHist;
AIDA::IHistogram2D * dutryyHist;
AIDA::IHistogram2D * dutryzHist;

AIDA::IHistogram2D * dutrzxHist;
AIDA::IHistogram2D * dutrzyHist;
AIDA::IHistogram2D * dutrzzHist;

AIDA::IHistogram2D * dutrxsxHist;
AIDA::IHistogram2D * dutrxsyHist;

AIDA::IHistogram2D * dutrysxHist;
AIDA::IHistogram2D * dutrysyHist;

AIDA::IHistogram2D * dutrzsxHist;
AIDA::IHistogram2D * dutrzsyHist;

AIDA::IHistogram2D * dutrxkxHist;
AIDA::IHistogram2D * dutrxkyHist;

AIDA::IHistogram2D * dutrykxHist;
AIDA::IHistogram2D * dutrykyHist;

AIDA::IHistogram2D * dutrzkxHist;
AIDA::IHistogram2D * dutrzkyHist;


AIDA::IHistogram2D * dutrxchi2ndfHist;
AIDA::IHistogram2D * dutrxprobHist;

AIDA::IHistogram2D * dutrychi2ndfHist;
AIDA::IHistogram2D * dutryprobHist;

AIDA::IHistogram2D * dutrzchi2ndfHist;
AIDA::IHistogram2D * dutrzprobHist;


AIDA::IProfile2D * dutkmap;
AIDA::IProfile2D * dutkxmap;
AIDA::IProfile2D * dutkymap;

AIDA::IHistogram2D * duttrackhitmap;

AIDA::IHistogram2D * duthitmap;

AIDA::IHistogram1D * gblax0Hist;
AIDA::IHistogram1D * gbldx0Hist;
AIDA::IHistogram1D * gblrx0Hist;
AIDA::IHistogram1D * gblay0Hist;
AIDA::IHistogram1D * gbldy0Hist;
AIDA::IHistogram1D * gblry0Hist;
AIDA::IHistogram1D * gblpx0Hist;
AIDA::IHistogram1D * gblpy0Hist;

AIDA::IHistogram1D * gblax1Hist;
AIDA::IHistogram1D * gbldx1Hist;
AIDA::IHistogram1D * gblrx1Hist;
AIDA::IHistogram1D * gblay1Hist;
AIDA::IHistogram1D * gbldy1Hist;
AIDA::IHistogram1D * gblry1Hist;
AIDA::IHistogram1D * gblpx1Hist;
AIDA::IHistogram1D * gblpy1Hist;

AIDA::IHistogram1D * gblax2Hist;
AIDA::IHistogram1D * gbldx2Hist;
AIDA::IHistogram1D * gblrx2Hist;
AIDA::IHistogram1D * gblay2Hist;
AIDA::IHistogram1D * gbldy2Hist;
AIDA::IHistogram1D * gblry2Hist;
AIDA::IHistogram1D * gblpx2Hist;
AIDA::IHistogram1D * gblpy2Hist;

AIDA::IHistogram1D * gblax3Hist;
AIDA::IHistogram1D * gbldx3Hist;
AIDA::IHistogram1D * gblrx3Hist;
AIDA::IHistogram1D * gblay3Hist;
AIDA::IHistogram1D * gbldy3Hist;
AIDA::IHistogram1D * gblry3Hist;
AIDA::IHistogram1D * gblpx3Hist;
AIDA::IHistogram1D * gblpy3Hist;

AIDA::IHistogram1D * gblax4Hist;
AIDA::IHistogram1D * gbldx4Hist;
AIDA::IHistogram1D * gblrx4Hist;
AIDA::IHistogram1D * gblay4Hist;
AIDA::IHistogram1D * gbldy4Hist;
AIDA::IHistogram1D * gblry4Hist;
AIDA::IHistogram1D * gblpx4Hist;
AIDA::IHistogram1D * gblpy4Hist;

AIDA::IHistogram1D * gblax5Hist;
AIDA::IHistogram1D * gbldx5Hist;
AIDA::IHistogram1D * gblrx5Hist;
AIDA::IHistogram1D * gblay5Hist;
AIDA::IHistogram1D * gbldy5Hist;
AIDA::IHistogram1D * gblry5Hist;
AIDA::IHistogram1D * gblpx5Hist;
AIDA::IHistogram1D * gblpy5Hist;

AIDA::IHistogram1D * gblax6Hist;
AIDA::IHistogram1D * gbldx6Hist;
AIDA::IHistogram1D * gblrx6Hist;
AIDA::IHistogram1D * gblay6Hist;
AIDA::IHistogram1D * gbldy6Hist;
AIDA::IHistogram1D * gblry6Hist;
AIDA::IHistogram1D * gblpx6Hist;
AIDA::IHistogram1D * gblpy6Hist;

AIDA::IHistogram1D * gblax7Hist;
AIDA::IHistogram1D * gbldx7Hist;
AIDA::IHistogram1D * gblrx7Hist;
AIDA::IHistogram1D * gblay7Hist;
AIDA::IHistogram1D * gbldy7Hist;
AIDA::IHistogram1D * gblry7Hist;
AIDA::IHistogram1D * gblpx7Hist;
AIDA::IHistogram1D * gblpy7Hist;

AIDA::IHistogram1D * gblkx1Hist;
AIDA::IHistogram1D * gblkx2Hist;
AIDA::IHistogram1D * gblkx3Hist;
AIDA::IHistogram1D * gblkx4Hist;
AIDA::IHistogram1D * gblkx5Hist;
AIDA::IHistogram1D * gblkx6Hist;
AIDA::IHistogram1D * gblkx7Hist;

AIDA::IHistogram1D * gblky1Hist;
AIDA::IHistogram1D * gblky2Hist;
AIDA::IHistogram1D * gblky3Hist;
AIDA::IHistogram1D * gblky4Hist;
AIDA::IHistogram1D * gblky5Hist;
AIDA::IHistogram1D * gblky6Hist;
AIDA::IHistogram1D * gblky7Hist;

AIDA::IHistogram1D * nmHist;
AIDA::IHistogram1D * nmGoodHist;

AIDA::IHistogram1D * tres01x;
AIDA::IHistogram1D * tres01y;
AIDA::IHistogram1D * tres01z;

AIDA::IHistogram1D * tres02x;
AIDA::IHistogram1D * tres02y;
AIDA::IHistogram1D * tres02z;

AIDA::IHistogram1D * tres12x;
AIDA::IHistogram1D * tres12y;
AIDA::IHistogram1D * tres12z;

AIDA::IHistogram2D * trackrate;
AIDA::IHistogram2D * badtrackrate;
AIDA::IHistogram2D * goodtrackrate;

TProfile * dutrxxProf;
TProfile * dutrxyProf;
TProfile * dutrxzProf;

TProfile * dutryxProf;
TProfile * dutryyProf;
TProfile * dutryzProf;

TProfile * dutrzxProf;
TProfile * dutrzyProf;
TProfile * dutrzzProf;

TF1 * profilefitalpha;
TF1 * profilefitbeta;
TF1 * profilefitgamma;

#endif

gbl::MilleBinary * milleGBL; // for producing MillePede-II binary file


EUTelMilleGBL::EUTelMilleGBL(): Processor("EUTelMilleGBL")
{
	//some default values
	FloatVec MinimalResidualsX;
	FloatVec MinimalResidualsY;
	FloatVec MaximalResidualsX;
	FloatVec MaximalResidualsY;

	FloatVec PedeUserStartValuesX;
	FloatVec PedeUserStartValuesY;

	FloatVec PedeUserStartValuesGamma;

	FloatVec SensorZPositions;

	FloatVec SensorXShifts;
	FloatVec SensorYShifts;

	FloatVec SensorGamma;
	FloatVec SensorAlpha;
	FloatVec SensorBeta;

	// 10 planes tops
	for ( int i=0; i<10; i++ )
	{
		MinimalResidualsX.push_back(0.0);
		MinimalResidualsY.push_back(0.0);
		MaximalResidualsX.push_back(0.0);
		MaximalResidualsY.push_back(0.0);

		PedeUserStartValuesX.push_back(0.0);
		PedeUserStartValuesY.push_back(0.0);

		PedeUserStartValuesGamma.push_back(0.0);

		// inital plane spacing 20mm
		float zpos = 20000.0 +  20000.0 * (float)i; // [um]
		SensorZPositions.push_back(zpos);

		SensorXShifts.push_back(0.0);
		SensorYShifts.push_back(0.0);

		SensorAlpha.push_back(0.0);
		SensorBeta.push_back(0.0);
		SensorGamma.push_back(0.0);
	}

	// modify processor description
	_description = "EUTelMilleGBL searches for tracks and fits them with GBL. Tracks are passed to MILLEPEDE II to get alignment constants and the fitted tracks and fitted hits are written to LCIO for possible further analysis.";

	// input collections
	std::vector<std::string > HitCollectionNameVecExample;
	HitCollectionNameVecExample.push_back("corrhits");

	registerInputCollections(LCIO::TRACKERHIT,"HitCollectionName","Hit collections names",_hitCollectionName,HitCollectionNameVecExample);

	// parameters
	registerProcessorParameter( "InputMode","Selects the source of input hits: 0: hits read from hitfile with simple trackfinding. 2: Test mode. Simple internal simulation and simple trackfinding.",_inputMode, static_cast <int> (0) );

	registerProcessorParameter( "Ebeam","Beam energy in GeV",_eBeam, static_cast < double >( 4.0) );

	// optional parameters
	EVENT::StringVec _alignmentCollectionNameExamples;
	_alignmentCollectionNameExamples.push_back("alignment");

	registerOptionalParameter ( "AlignmentCollectionName", "All alignment collection names which have been applied must be entered here. The order should correspond to their application, i.e.: alignment1 alignment2 etc.", _alignmentCollectionName, _alignmentCollectionNameExamples );

	registerOptionalParameter( "AlignMode","Number of alignment constants used. Available mode are: 1: shift in Z only. 2: shifts in X and Y. 3: shifts in X and Y and rotation around the Z axis, 4: shifts in X,Y and Z and rotation around the Z axis. 5: shifts in X,Y and rotations around all axis, fix Z. 6: shifts in X,Y and Z and rotations around all axis. 7: Shifts in X, Y and Z. Note that in modes 5 and 6, alignment constants in alpha and beta will only be returned for rotated/tilted sensors.", _alignMode, static_cast <int> (3) );

	registerOptionalParameter( "AlignmentConstantLCIOFile","This is the name of the LCIO file name with the output alignment",_alignmentConstantLCIOFile, static_cast< string > ( "alignment.slcio" ) );

	registerOptionalParameter( "AlignmentConstantCollectionName","This is the name of the alignment collection to be saved into the slcio file",_alignmentConstantCollectionName, static_cast< string > ( "alignment" ) );

	registerOptionalParameter( "BinaryFilename","Name of the Millepede binary output file.",_binaryFilename, string ("mille.bin") );

	registerOptionalParameter( "Chi2NdfCut","Cut in Chi2/Ndf, tracks below are accepted", _chi2ndfCut, 10.0 );

	registerOptionalParameter ( "CoordinatorPreAlignmentCollectionName", "The Coordinator PreAlignment to be loaded", _coordinatorPreAlignmentCollectionName, static_cast< string > ("coordinatorprealignment") );

	registerOptionalParameter( "DoDUTAlignment","Do DUT Alignment? 1 = a, 2 = b, 3 = g, 0 = no", _doDUTAlignment, 0 );

	registerOptionalParameter( "DoPreAlignment","Do Prealignment? 1 = yes, 0 = no, 2 = yes, with zero Prealignment", _doPreAlignment, 0 );

	registerOptionalParameter ( "DUTFitMethod", "Methods with which to get the DUT fit position: 0: centre of the DUT (bad if DUT is rotated), 1: extrapolate from triplet and get intersect with plane, 2: extrapolate from driplet, 3: get average from methods 1 and 2.", _dutFitMethod, static_cast < int > (3) );

	registerOptionalParameter( "DUTResX","DUT resolution in x [um]", _resdutx, 55.0 );

	registerOptionalParameter( "DUTResY","DUT resolution in y [um]", _resduty, 25.0 );

	registerOptionalParameter( "ExcludePlanes","Exclude planes from fit according to their sensor ids.",_excludePlanes_sensorIDs ,std::vector<int>() );

	registerOptionalParameter( "FixedPlanes","Fix sensor planes in the fit according to their sensor ids.",_FixedPlanes_sensorIDs ,std::vector<int>() );

	registerOptionalParameter("FixParameter","Fixes the given alignment parameters in the fit. For each sensor an integer must be specified (If no value is given, then all parameters will be free). bit 0 = x shift, bit 1 = y shift, bit 2 = z shift, bit 3 = alpha, bit 4 = beta, bit 5 = gamma. Note: these numbers are ordered according to the z position of the sensors and NOT according to the sensor id.",_FixParameter, IntVec (static_cast <int> (6), 24));

	registerOptionalParameter( "GeneratePedeSteerfile","Generate a steering file for the pede program? 0 = false, 1 = true.",_generatePedeSteerfile, static_cast <int> (0) );

	registerOptionalParameter( "ManualDUTid", "The id number of the DUT (usually 6).", _manualDUTid, static_cast < int > (6) );

	registerOptionalParameter( "MaxTrackCandidatesTotal","Maximal number of track candidates in the whole run.",_maxTrackCandidatesTotal, static_cast <int> (10000000) );

	registerOptionalParameter( "OutputHitCollection","The name of the output fit hit collection", _outputHitCollectionName, std::string("fithits") );

	registerOptionalParameter( "OutputTrackCollection","The name of the output track fit collection", _outputTrackCollectionName, std::string("fittracks") );

	registerOptionalParameter( "PedeSteerfileName","Name of the steering file for the pede program.",_pedeSteerfileName, string("steer_mille.txt") );

	EVENT::StringVec _pre_alignmentCollectionNameExamples;
	_pre_alignmentCollectionNameExamples.push_back("prealign");
	
	registerOptionalParameter ( "PreAlignmentCollectionName", "All prealignment collection names which have been applied must be entered here. The order should correspond to their application, i.e.: prealignment1 prealignment2 etc.", _pre_alignmentCollectionName, _pre_alignmentCollectionNameExamples );

	registerOptionalParameter( "ProbCut","Probability Cut, tracks above are accepted", _probCut, 0.01 );

	registerOptionalParameter( "REFResX","REF resolution in x [um]", _resrefx, 10.0 );

	registerOptionalParameter( "REFResY","REF resolution in y [um]", _resrefy, 10.0 );

	registerOptionalParameter( "RequireDUTHit","Require DUT hit for valid track? 0 = false, 1 = true.",_requireDUTHit, static_cast <int> (1) );

	registerOptionalParameter( "RunPede","Execute the pede program using the generated steering file? 0 = false, 1 = true.",_runPede, static_cast <int> (0) );

	registerOptionalParameter( "SlopeCutDUTx","Track slope cut in x, tracks below are accepted", _slopecutDUTx, 10.0 );

	registerOptionalParameter( "SlopeCutDUTy","Track slope cut in y, tracks below are accepted", _slopecutDUTy, 10.0 );

	registerOptionalParameter( "TelescopeResX","Telescope resolution in x [um]", _resx, 15.0 );

	registerOptionalParameter( "TelescopeResY","Telescope resolution in y [um]", _resy, 15.0 );

	registerOptionalParameter( "TestModeSensorAlpha","Rotation around the x axis of the sensors in test mode (to be determined by the alignment).",_testModeSensorAlpha,SensorAlpha );

	registerOptionalParameter( "TestModeSensorBeta","Rotation around the y axis of the sensors in test mode (to be determined by the alignment).",_testModeSensorBeta,SensorBeta );

	registerOptionalParameter( "TestModeSensorGamma","Rotation around the z axis of the sensors in test mode (to be determined by the alignment).",_testModeSensorGamma,SensorGamma );

	registerOptionalParameter( "TestModeSensorResolution","Resolution assumed for the sensors in test mode.",_testModeSensorResolution, static_cast <float> (3.0) );

	registerOptionalParameter( "TestModeSensorXShifts","X shifts of the sensors in test mode (to be determined by the alignment).",_testModeSensorXShifts,SensorXShifts );

	registerOptionalParameter( "TestModeSensorYShifts","Y shifts of the sensors in test mode (to be determined by the alignment).",_testModeSensorYShifts,SensorYShifts );

	registerOptionalParameter( "TestModeSensorZPositions","Z positions of the sensors in test mode.",_testModeSensorZPositions,SensorZPositions );

	registerOptionalParameter( "TestModeXTrackSlope","Width of the track slope distribution in the x direction in test mode.",_testModeXTrackSlope, static_cast <float> (0.0005) );

	registerOptionalParameter( "TestModeYTrackSlope","Width of the track slope distribution in the y direction in test mode.",_testModeYTrackSlope, static_cast <float> (0.0005) );

	registerOptionalParameter( "TimeDependencyHistos","Flag to fill dt histograms. Will increase file size quite a bit.",_dthistos, false);

	registerOptionalParameter( "UseREF","Use Reference Plane?", _useREF, false );

	registerOptionalParameter( "driCut", "Downstream triplet residual cut [um]", _driCut, 400.0 );

	registerOptionalParameter( "driCutREFx", "Downstream driplet REF residual cut x [um]", _driCutREFx, 3000.0 );

	registerOptionalParameter( "driCutREFy", "Downstream driplet REF residual cut y [um]", _driCutREFy, 3000.0 );

	registerOptionalParameter( "sixCut", "Upstream-Downstream Track matching cut [um]", _sixCut, 600.0 );

	registerOptionalParameter( "triCut", "Upstream triplet residual cut [um]", _triCut, 300.0 );

	registerOptionalParameter( "triCutDUTx", "Upstream triplet DUT residual cut x [um]", _triCutDUTx, 3000.0 );

	registerOptionalParameter( "triCutDUTy", "Upstream triplet DUT residual cut y [um]", _triCutDUTy, 3000.0 );

}


void EUTelMilleGBL::init()
{

	// check if Marlin was built with GEAR support or not
	#ifndef USE_GEAR

		streamlog_out( ERROR2 ) << "Marlin was not built with GEAR support." << endl;
		streamlog_out( ERROR2 ) << "You need to install GEAR and recompile Marlin with -DUSE_GEAR before continue." << endl;

		exit(-1);

	#else

	// check if the GEAR manager pointer is not null!
	if( Global::GEAR == 0x0 )
	{
		streamlog_out( ERROR2) << "The GearMgr is not available, for an unknown reason." << endl;
		exit(-1);
	}

	_siPlanesParameters  = const_cast<gear::SiPlanesParameters* > (&(Global::GEAR->getSiPlanesParameters()));
	_siPlanesLayerLayout = const_cast<gear::SiPlanesLayerLayout*> ( &(_siPlanesParameters->getSiPlanesLayerLayout() ));

	const gear::BField& Bfield = Global::GEAR->getBField();
	gear::Vector3D vectorGlobal(0.1,0.1,0.1);
	const double Bx = (Bfield.at( vectorGlobal ).x());
	const double By = (Bfield.at( vectorGlobal ).y());
	const double Bz = (Bfield.at( vectorGlobal ).z());
	TVector3 B(Bx, By, Bz);
	if (fabs(Bx) > 1e-6 || fabs(By) > 1e-6 || fabs(By) > 1e-6 )
	{
		_BField = true;
		streamlog_out(MESSAGE4) << "Running WITH magnetic field! " << Bx << " Bx, " << By << " By, " << Bz << " Bz!" <<  endl;
		streamlog_out(MESSAGE4) << "The residual cuts will be multiplied by 10!" << endl;
		_driCut = _driCut * 10.0;
		_driCutREFx = _driCutREFx * 10.0;
		_driCutREFy = _driCutREFy * 10.0;
		_sixCut = _sixCut * 10.0;
		_triCut = _triCut * 10.0;
		_triCutDUTx = _triCutDUTx* 10.0;
		_triCutDUTy = _triCutDUTy * 10.0;
		streamlog_out(MESSAGE4) << "The chi2, slope and prob cuts will be multiplied by 5!" << endl;
		_chi2ndfCut = _chi2ndfCut * 5.0;
		_probCut = _probCut * 0.2;
		_slopecutDUTx = _slopecutDUTx * 5.0;
		_slopecutDUTy = _slopecutDUTy * 5.0;
	} else {
		_BField = false;
		streamlog_out(MESSAGE4) << "Running WITHOUT magnetic field!" << endl;
	}

	streamlog_out(MESSAGE4) << "Assumed beam energy " << _eBeam << " GeV" <<  endl;

	//lets guess the number of planes
	if( _inputMode == 0 || _inputMode == 2 )
	{

		// the number of planes is got from the GEAR description and is
		// the sum of the telescope reference planes and the DUT (if any)
		_nPlanes = _siPlanesParameters->getSiPlanesNumber();
		if( _siPlanesParameters->getSiPlanesType() == _siPlanesParameters->TelescopeWithDUT )
		{
			++_nPlanes;
		}
	} else
	{
		streamlog_out (ERROR2) << "Unknown input mode: " << _inputMode << endl;
		exit(-1);
	}

	// an associative map for getting also the sensorID ordered
	map< double, int > sensorIDMap;

	//lets create an array with the z positions of each layer
	for(  int iPlane = 0; iPlane < _siPlanesLayerLayout->getNLayers(); iPlane++ )
	{
		_siPlaneZPosition.push_back(_siPlanesLayerLayout->getLayerPositionZ(iPlane));
		sensorIDMap.insert( make_pair( _siPlanesLayerLayout->getLayerPositionZ(iPlane), _siPlanesLayerLayout->getID(iPlane) ) );
	}

	if( _siPlanesParameters->getSiPlanesType() == _siPlanesParameters->TelescopeWithDUT )
	{
		_siPlaneZPosition.push_back(_siPlanesLayerLayout->getDUTPositionZ());
		sensorIDMap.insert( make_pair( _siPlanesLayerLayout->getDUTPositionZ(),  _siPlanesLayerLayout->getDUTID() ) ) ;
  }

	//lets sort the array with increasing z
	sort(_siPlaneZPosition.begin(), _siPlaneZPosition.end());

	streamlog_out (MESSAGE4) << "Planes sorted along z:" << endl;
	for( size_t i = 0; i < _siPlaneZPosition.size(); i++ )
	{
		streamlog_out (MESSAGE4) << i << "  z " << _siPlaneZPosition[i] << " mm" << endl;
	}

	// the user is giving sensor ids for the planes to be excluded.
	// These sensor ids have to be converted to a local index
	// according to the planes positions along the z axis.

	streamlog_out (MESSAGE4) << "Fixed planes: " << _FixedPlanes_sensorIDs.size() << endl;
	for( size_t i = 0; i < _FixedPlanes_sensorIDs.size(); i++ )
	{
		streamlog_out (MESSAGE4) << "Plane " << _FixedPlanes_sensorIDs[i] << " fixed\n";
		map< double, int >::iterator iter = sensorIDMap.begin();
		int counter = 0;
		while ( iter != sensorIDMap.end() )
		{
			if( iter->second == _FixedPlanes_sensorIDs[i] )
			{
				_FixedPlanes.push_back(counter);
				break;
			}
			++iter;
			++counter;
		}
	}

	streamlog_out (MESSAGE4) << "Excluded planes: " << _excludePlanes_sensorIDs.size() << endl;
	for( size_t i = 0; i < _excludePlanes_sensorIDs.size(); i++ )
	{
		streamlog_out (MESSAGE4) << "Plane " << _excludePlanes_sensorIDs[i] << " excluded\n";
		map< double, int >::iterator iter = sensorIDMap.begin();
		int counter = 0;
		while ( iter != sensorIDMap.end() )
		{
			if( iter->second == _excludePlanes_sensorIDs[i])
			{
				_excludePlanes.push_back(counter);
				break;
			}
			++iter;
			++counter;
		}
	}

	// strip from the map the sensor id already sorted.
	map< double, int >::iterator iter = sensorIDMap.begin();
	int counter = 0;
	while ( iter != sensorIDMap.end() )
	{
		bool excluded = false;
		for( size_t i = 0; i < _excludePlanes.size(); i++)
		{
			if(_excludePlanes[i] == counter)
			{
				// printf("excludePlanes %2d of %2d (%2d) \n", i, _excludePlanes_sensorIDs.size(), counter );
				excluded = true;
				break;
			}
		}
		if(!excluded)
		{
			_orderedSensorID_wo_excluded.push_back( iter->second );
		}

		_orderedSensorID.push_back( iter->second );
		++iter;
		++counter;

	}

	//consistency
	if( (int)_siPlaneZPosition.size() != _nPlanes )
	{
		streamlog_out( ERROR2 ) << "The number of detected planes is " << _nPlanes << " but only " << _siPlaneZPosition.size() << " layer z positions were found!"  << endl;
		exit(-1);
	}

	#endif

	if(_FixParameter.size() < static_cast<unsigned int>(_nPlanes ) && !_FixParameter.empty())
	{
		streamlog_out ( WARNING2 ) << "Consistency check of the fixed parameters array failed. The array size is smaller than the number of found planes! The array is now set to default values, which means that all parameters are free in the fit!" << endl;
		_FixParameter.clear();
		for(int i = 0; i < _nPlanes; i++)
		{
			_FixParameter.push_back(0);
		}
	}
	if(_FixParameter.empty())
	{
		streamlog_out ( WARNING2 ) << "The fixed parameters array was found to be empty. It will be filled with default values. All parameters are free in the fit now." << endl;
		_FixParameter.clear();
		for(int i = 0; i < _nPlanes; i++)
		{
			_FixParameter.push_back(0);
		}
        }

	// this method is called only once even when the rewind is active
	// usually a good idea to
	printParameters ();

	// set to zero the run and event counters
	_iRun = 0;
	_iEvt = 0;

	// Initialize number of excluded planes
	_nExcludePlanes = _excludePlanes.size();

	streamlog_out( MESSAGE4 ) << "Number of planes excluded from the alignment fit: " << _nExcludePlanes << endl;

	// Initialise Mille statistics:
	_nMilleTracks = 0;
	_nGoodMilleTracks = 0;

	// booking histograms
	bookHistos();

	streamlog_out( MESSAGE4 ) << "Initialising Mille..." << endl;

	unsigned int reserveSize = 8000;
	milleGBL = new gbl::MilleBinary( _binaryFilename, reserveSize );

	streamlog_out( MESSAGE4 ) << "The filename for the binary file is: " << _binaryFilename.c_str() << endl;


	// allocate arrays for alignment constants of the dut
	_dut_align_x = new double[_alignmentCollectionName.size()];
	_dut_align_y = new double[_alignmentCollectionName.size()];
	_dut_align_z = new double[_alignmentCollectionName.size()];
	_dut_align_a = new double[_alignmentCollectionName.size()];
	_dut_align_b = new double[_alignmentCollectionName.size()];
	_dut_align_c = new double[_alignmentCollectionName.size()];
	_dut_align_x_error = new double[_alignmentCollectionName.size()];
	_dut_align_y_error = new double[_alignmentCollectionName.size()];
	_dut_align_z_error = new double[_alignmentCollectionName.size()];
	_dut_align_a_error = new double[_alignmentCollectionName.size()];
	_dut_align_b_error = new double[_alignmentCollectionName.size()];
	_dut_align_c_error = new double[_alignmentCollectionName.size()];
	_dut_pre_align_x = new double[_pre_alignmentCollectionName.size()];
	_dut_pre_align_y = new double[_pre_alignmentCollectionName.size()];
	_dut_pre_align_z = new double[_pre_alignmentCollectionName.size()];
	_dut_pre_align_a = new double[_pre_alignmentCollectionName.size()];
	_dut_pre_align_b = new double[_pre_alignmentCollectionName.size()];
	_dut_pre_align_c = new double[_pre_alignmentCollectionName.size()];

	_Xaxis[0] = 1.0;
	_Xaxis[1] = 0.0;
	_Xaxis[2] = 0.0;

	_Yaxis[0] = 0.0;
	_Yaxis[1] = 1.0;
	_Yaxis[2] = 0.0;

	_Zaxis[0] = 0.0;
	_Zaxis[1] = 0.0;
	_Zaxis[2] = 1.0;

	_alignmentloaded = false;
	_pre_alignmentloaded = false;
	_dutplanevectors = false;

	_chi2ndfCutCount = 0;
	_probCutCount = 0;
	_cutfailsx = 0;
	_cutfailsy = 0;

	streamlog_out( MESSAGE4 ) << "End of init" << endl;

}

void EUTelMilleGBL::getDUTNormal ()
{

	// The vector to the DUT and the DUT normalvector
	_dutplane[0] = 0.0 - _coordinator_x;
	_dutplane[1] = 0.0 - _coordinator_y;
	_dutplane[2] = 0.0 - _coordinator_z;
	double alpha = _siPlanesLayerLayout->getLayerRotationZY(3); // HACK
	double beta  = _siPlanesLayerLayout->getLayerRotationZX(3);
	// normalvector should be invariant to rotations around z...
	//double gamma = _siPlanesLayerLayout->getLayerRotationXY(6);

	// the components of the plane normal vector rotated by gear angles
	// if all angles are zero, this should be ( 0 | 0 | 1 )
	double n_x = sin(beta*PI/180.0);
	double n_y = -sin(alpha*PI/180.0);
	double n_z = cos(alpha*PI/180.0)*cos(beta*PI/180.0);
	_normalvector[0]=n_x;
	_normalvector[1]=n_y;
	_normalvector[2]=n_z;

	// now we have gear shifts and rotations for both plane start vector and plane normal vector
	// both need to be rotated with the alignment rotations
	// the start vector also needs the shifts, the normal vector should be invariant against this

	// the prealignment gets added...
	for (unsigned int i=0;i<_pre_alignmentCollectionName.size();i++)
	{
		streamlog_out ( DEBUG2 ) << "Performing prealignment on DUT plane vector, step " << i+1 << " !" << endl;

		// add rotation -> negative rotation
		_dutplane.Rotate(-_dut_pre_align_a[i],_Xaxis);
		_dutplane.Rotate(-_dut_pre_align_b[i],_Yaxis);
		_dutplane.Rotate(-_dut_pre_align_c[i],_Zaxis);

		_normalvector.Rotate(-_dut_pre_align_a[i],_Xaxis);
		_normalvector.Rotate(-_dut_pre_align_b[i],_Yaxis);
		_normalvector.Rotate(-_dut_pre_align_c[i],_Zaxis);

		// add shift -> subtract it
		TVector3 alignshift(_dut_pre_align_x[i],_dut_pre_align_y[i],_dut_pre_align_z[i]);
		_dutplane = _dutplane - alignshift;
	}

	// the alignment gets added
	for (unsigned int i=0;i<_alignmentCollectionName.size();i++)
	{
		streamlog_out ( DEBUG2 ) << "Performing alignment on DUT plane vector, step " << i+1 << " !" << endl;

		// add rotation -> negative rotation
		_dutplane.Rotate(-_dut_align_a[i],_Xaxis);
		_dutplane.Rotate(-_dut_align_b[i],_Yaxis);
		_dutplane.Rotate(-_dut_align_c[i],_Zaxis);

		_normalvector.Rotate(-_dut_align_a[i],_Xaxis);
		_normalvector.Rotate(-_dut_align_b[i],_Yaxis);
		_normalvector.Rotate(-_dut_align_c[i],_Zaxis);

		// add shift -> subtract it
		TVector3 alignshift(_dut_align_x[i],_dut_align_y[i],_dut_align_z[i]);
		_dutplane = _dutplane - alignshift;

	}
	
	// HACK
	TVector3 initialpos(0.0,0.0,_siPlaneZPosition[3] + 0.5 *_siPlanesLayerLayout->getSensitiveThickness(3));
	_dutplane = _dutplane + initialpos;
	// go to um
	_dutplane[0] = _dutplane[0]*1000.0;
	_dutplane[1] = _dutplane[1]*1000.0;
	_dutplane[2] = _dutplane[2]*1000.0;
	streamlog_out ( MESSAGE4 ) << "**************************************************" << endl;
	streamlog_out ( MESSAGE4 ) << "DUT plane position: X: " << _dutplane[0] << " um, Y: " << _dutplane[1] << " um, Z: "<< _dutplane[2] << " um" << endl;
	streamlog_out ( MESSAGE4 ) << "DUT plane normal:   X: " << _normalvector[0] << " , Y: " << _normalvector[1] << " , Z: "<< _normalvector[2] << endl;

}


bool EUTelMilleGBL::isPointInPlane (TVector3 point)
{
	// a quick function to test if a point is on the (rotated) dut plane
	double dist = 0.0;
	dist = _normalvector.Dot(point - _dutplane);
	if (dist < 0.001)
	{
		return(true);
	} else {
		//streamlog_out (MESSAGE4) << "point NOT in plane "<< endl;
		streamlog_out ( DEBUG3 ) << "point  " << point[0] << " " << point[1] << " " << point[2] <<" 's distance is " << dist << endl;
		streamlog_out ( DEBUG3 ) << "plane  " << _dutplane[0] << " " << _dutplane[1] << " " << _dutplane[2] << endl;
		streamlog_out ( DEBUG3 ) << "normal " << _normalvector[0] << " " << _normalvector[1] << " " << _normalvector[2] << endl;
		return(false);
	}
}


void EUTelMilleGBL::getAlignment (LCEvent * event)
{

	// Get the alignment from file
	// assumes rotations in rad!

	LCCollectionVec * alignmentCollection;

	for (size_t i =0; i<_alignmentCollectionName.size(); i++)
	{

		try {
			alignmentCollection = dynamic_cast< LCCollectionVec * > ( event->getCollection( _alignmentCollectionName[i] ) );
		
			for (int j=0; j<_nPlanes;j++)
			{
				 EUTelAlignmentConstant * alignment = static_cast< EUTelAlignmentConstant * > ( alignmentCollection->getElementAt( j ) );

				 if (alignment->getSensorID() == _manualDUTid)
				 {
					_dut_align_x[i] = alignment->getXOffset();
					_dut_align_y[i] = alignment->getYOffset();
					_dut_align_z[i] = alignment->getZOffset();
					_dut_align_a[i] = alignment->getAlpha();
					_dut_align_b[i] = alignment->getBeta();
					_dut_align_c[i] = alignment->getGamma();
					_dut_align_x_error[i] = alignment->getXOffsetError();
					_dut_align_y_error[i] = alignment->getYOffsetError();
					_dut_align_z_error[i] = alignment->getZOffsetError();
					_dut_align_a_error[i] = alignment->getAlphaError();
					_dut_align_b_error[i] = alignment->getBetaError();
					_dut_align_c_error[i] = alignment->getGammaError();
				}
			}
		} catch (lcio::DataNotAvailableException& e) {
			streamlog_out( ERROR2 ) << "No alignment collection with name " << _alignmentCollectionName[i] << " !" << endl;
			return;
		}

		streamlog_out ( DEBUG2 ) << "**************************************************" << endl;
		streamlog_out ( DEBUG2 ) << "Alignment loading, iteration " << i+1 << ":" << endl;
		streamlog_out ( DEBUG2 ) << "DUT alignment: X is:     " << _dut_align_x[i] << endl;
		streamlog_out ( DEBUG2 ) << "DUT alignment: Y is:     " << _dut_align_y[i] << endl;
		streamlog_out ( DEBUG2 ) << "DUT alignment: Z is:     " << _dut_align_z[i] << endl;
		streamlog_out ( DEBUG2 ) << "DUT alignment: Alpha is: " << _dut_align_a[i] << endl;
		streamlog_out ( DEBUG2 ) << "DUT alignment: Beta is:  " << _dut_align_b[i] << endl;
		streamlog_out ( DEBUG2 ) << "DUT alignment: Gamma is: " << _dut_align_c[i] << endl;
	}
}


void EUTelMilleGBL::getPreAlignment (LCEvent * event)
{

	// Get the alignment from file
	// assumes rotations in rad!

	LCCollectionVec * pre_alignmentCollection;

	for (size_t i =0; i<_pre_alignmentCollectionName.size(); i++)
	{
		try {
			pre_alignmentCollection = dynamic_cast< LCCollectionVec * > ( event->getCollection( _pre_alignmentCollectionName[i] ) );

			for (int j=0; j<_nPlanes;j++)
			{
				 EUTelAlignmentConstant * pre_alignment = static_cast< EUTelAlignmentConstant * > ( pre_alignmentCollection->getElementAt( j ) );

				 if (pre_alignment->getSensorID() == _manualDUTid)
				 {
					_dut_pre_align_x[i] = pre_alignment->getXOffset();
					_dut_pre_align_y[i] = pre_alignment->getYOffset();
					_dut_pre_align_z[i] = pre_alignment->getZOffset();
					_dut_pre_align_a[i] = pre_alignment->getAlpha();
					_dut_pre_align_b[i] = pre_alignment->getBeta();
					_dut_pre_align_c[i] = pre_alignment->getGamma();
				 }
			}
		} catch (lcio::DataNotAvailableException& e) {
			streamlog_out( ERROR2 ) << "No pre_alignment collection with name " << _pre_alignmentCollectionName[i] << " !" << endl;
			return;
		}

		streamlog_out ( DEBUG2 ) << "**************************************************" << endl;
		streamlog_out ( DEBUG2 ) << "Prealignment loading, iteration " << i+1 << ":" << endl;
		streamlog_out ( DEBUG2 ) << "DUT pre_alignment: X is:     " << _dut_pre_align_x[i] << endl;
		streamlog_out ( DEBUG2 ) << "DUT pre_alignment: Y is:     " << _dut_pre_align_y[i] << endl;
		streamlog_out ( DEBUG2 ) << "DUT pre_alignment: Z is:     " << _dut_pre_align_z[i] << endl;
		streamlog_out ( DEBUG2 ) << "DUT pre_alignment: Alpha is: " << _dut_pre_align_a[i] << endl;
		streamlog_out ( DEBUG2 ) << "DUT pre_alignment: Beta is:  " << _dut_pre_align_b[i] << endl;
		streamlog_out ( DEBUG2 ) << "DUT pre_alignment: Gamma is: " << _dut_pre_align_c[i] << endl;
	}
}


void EUTelMilleGBL::getCoordinatorAlignment (LCEvent * event)
{

	// Get the alignment from file
	// assumes rotations in rad!

	LCCollectionVec * coordinatorCollection;

	try {
		coordinatorCollection = dynamic_cast< LCCollectionVec * > ( event->getCollection( _coordinatorPreAlignmentCollectionName ) );

		for (int j=0; j<_nPlanes;j++)
		{
			EUTelAlignmentConstant * coordinator_alignment = static_cast< EUTelAlignmentConstant * > ( coordinatorCollection->getElementAt( j ) );

			if (coordinator_alignment->getSensorID() == _manualDUTid)
			{
				_coordinator_x = coordinator_alignment->getXOffset();
				_coordinator_y = coordinator_alignment->getYOffset();
				_coordinator_z = coordinator_alignment->getZOffset();
			}
		}
	} catch (lcio::DataNotAvailableException& e) {
		streamlog_out( ERROR2 ) << "No coordinator_alignment collection with name " << _coordinatorPreAlignmentCollectionName << " !" << endl;
		return;
	}

	streamlog_out ( DEBUG2 ) << "**************************************************" << endl;
	streamlog_out ( DEBUG2 ) << "DUT coordinator_alignment: X is:     " << _coordinator_x << endl;
	streamlog_out ( DEBUG2 ) << "DUT coordinator_alignment: Y is:     " << _coordinator_y << endl;
	streamlog_out ( DEBUG2 ) << "DUT coordinator_alignment: Z is:     " << _coordinator_z << endl;
}


void EUTelMilleGBL::processRunHeader( LCRunHeader * rdr )
{

	auto_ptr<EUTelRunHeaderImpl> header ( new EUTelRunHeaderImpl (rdr) );
	header->addProcessor( type() ) ;

	// this is the right place also to check the geometry ID. This is a
	// unique number identifying each different geometry used at the
	// beam test. The same number should be saved in the run header and
	// in the xml file. If the numbers are different, instead of barely
	// quitting ask the user what to do.

	if( header->getGeoID() != _siPlanesParameters->getSiPlanesID() )
	{
		streamlog_out( ERROR2 ) << "Error during the geometry consistency check: " << endl;
		streamlog_out( ERROR2 ) << "The run header says the GeoID is " << header->getGeoID() << endl;
		streamlog_out( ERROR2 ) << "The GEAR description says is     " << _siPlanesParameters->getSiPlanesNumber() << endl;

		#ifdef EUTEL_INTERACTIVE
		string answer;
		while (true)
		{
			streamlog_out( ERROR2 ) << "Type Q to quit now or C to continue using the actual GEAR description anyway [Q/C]" << endl;
			cin >> answer;

			// put the answer in lower case before making the comparison.
			transform( answer.begin(), answer.end(), answer.begin(), ::tolower );
			if( answer == "q" )
			{
				exit(-1);
			} else if ( answer == "c" )
			{
				break;
			}
		}
		#endif
	}

	// increment the run counter
	++_iRun;
}

TMatrixD Jac55( double ds, TVector3 direction, bool bfield)
{
	TMatrixD jac(5, 5);
	if (bfield == true)
	{
		if (direction[2] < 1e-6)
		{
			direction[2] = 1e-6;
		}
		streamlog_out ( DEBUG5 ) << "Path length: " << endl;
		streamlog_out ( DEBUG5 ) << " " << ds << " um" <<endl;
		// expects mm
		ds = ds / 1000.0;
		jac.UnitMatrix();
		direction.Unit();
		std::vector<double> slope;
		slope.push_back(direction[0]/direction[2]);
		slope.push_back(direction[1]/direction[2]);
		double norm = std::sqrt(pow(slope.at(0),2) + pow(slope.at(1),2) + 1);
		TVector3 direction_n;
		direction_n[0] = (slope.at(0)/norm);
		direction_n[1] = (slope.at(1)/norm);
		direction_n[2] = (1.0/norm);
		streamlog_out ( DEBUG5 ) << "Direction:" << endl;
		streamlog_out ( DEBUG5 ) << " X " << direction[0] << " Y " << direction[1]<< " Z " << direction[2] << endl;
		streamlog_out ( DEBUG5 ) << "Normalised Direction:" << endl;
		streamlog_out ( DEBUG5 ) << " X " << direction_n[0] << " Y " << direction_n[1]<< " Z " << direction_n[2] << endl;
		double sinLambda = direction_n[2];
		const gear::BField& Bfield = geo::gGeometry().getMagneticField();
		gear::Vector3D vectorGlobal(1.0,1.0,1.0);

		// expects kG -> input times 10
		const double Bx = (Bfield.at( vectorGlobal ).x()*10);
		const double By = (Bfield.at( vectorGlobal ).y()*10);
		const double Bz = (Bfield.at( vectorGlobal ).z()*10);

		TVector3 b(Bx, By, Bz);
		TVector3 BxT = b.Cross(direction_n);
		streamlog_out ( DEBUG5 ) << "BxT:" << endl;
		streamlog_out ( DEBUG5 ) << " X " << BxT[0] << " Y " << BxT[1] << " Z " << BxT[2] << endl;
		TMatrixD xyDir(2, 3);
		xyDir[0][0] = 1.0;
		xyDir[0][1] = 0.0;
		xyDir[0][2] = -slope.at(0);
		xyDir[1][0] = 0;
		xyDir[1][1] = 1.0;
		xyDir[1][2] = -slope.at(1);

		streamlog_out( DEBUG5 ) << "Propagator (Dx,Dy):" << endl;
		streamlog_out( DEBUG5 ) << " " << xyDir[0][0] << " " << xyDir[0][1] << " " << xyDir[0][2] << endl;
		streamlog_out( DEBUG5 ) << " " << xyDir[1][0] << " " << xyDir[1][1] << " " << xyDir[1][2] << endl;

		TMatrixD bFac(2,1);
		TMatrixD BxTMatrix(3,1);
		BxTMatrix.Zero();
		BxTMatrix[0][0] = BxT[0];
		BxTMatrix[1][0] = BxT[1];
		BxTMatrix[2][0] = BxT[2]; 

		double kappa = -0.00002998;
		bFac = kappa * (xyDir*BxTMatrix); 
		streamlog_out ( DEBUG5 ) << "bFac" << endl;
		streamlog_out ( DEBUG5 ) << " " << bFac[0][0] << " " <<  bFac[1][0] << endl;

		jac[1][0] = bFac[0][0]*ds/sinLambda;
		streamlog_out ( DEBUG5 ) << "Jacobian entries:"<< endl;
		streamlog_out ( DEBUG5 ) << "1,0 " << jac[1][0] << endl;
		jac[2][0] = bFac[1][0]*ds/sinLambda;
		streamlog_out ( DEBUG5 ) << "2,0 " << jac[2][0] << endl;
		jac[3][0] = 0.5*bFac[0][0]*ds*ds;
		streamlog_out ( DEBUG5 ) << "3,0 " << jac[3][0] << endl;
		jac[4][0] = 0.5*bFac[1][0]*ds*ds;
		streamlog_out ( DEBUG5 ) << "4,0 " << jac[4][0] << endl;
		jac[3][1] = ds*sinLambda;
		streamlog_out ( DEBUG5 ) << "3,1 " << jac[3][1] << endl;
		jac[4][2] = ds*sinLambda;
		streamlog_out ( DEBUG5 ) << "4,2 " << jac[4][2] << endl;

	}
	if (bfield == false)
	{
		/* for GBL:
			Jacobian for straight line track
			track:	q/p	x'	y'	x	y
				0	1	2	3	4
				
			matrix:
				1	0	0	0	0
				0	1	0	0	0
				0	0	1	0	0
				0	ds	0	1	0
				0	0	ds	0	1
		*/
		jac.UnitMatrix();
		jac[3][1] = ds; // x = x0 + xp * ds
		jac[4][2] = ds; // y = y0 + yp * ds
		
	}
	return jac;

}

void EUTelMilleGBL::processEvent( LCEvent * event )
{

	// the output fit hits
	LCCollectionVec *fittrackvec = new LCCollectionVec(LCIO::TRACK);
	LCCollectionVec *fitpointvec = new LCCollectionVec(LCIO::TRACKERHIT);

	// load the alignment only once
	if (_alignmentloaded == false)
	{
		getAlignment(event);
		_alignmentloaded = true;
		streamlog_out ( MESSAGE2 ) <<  "DUT Alignment loaded..." << endl;
	}

	// also load prealignment only once
	if (_pre_alignmentloaded == false)
	{
		getPreAlignment(event);
		getCoordinatorAlignment(event);
		_pre_alignmentloaded = true;
		streamlog_out ( MESSAGE2 ) <<  "DUT Prealignment loaded..." << endl;
	}

	// dut plane vectors
	if (_dutplanevectors == false)
	{
		getDUTNormal();
		_dutplanevectors = true;
		streamlog_out ( MESSAGE2 ) <<  "DUT position calculated..." << endl;
	}

	if( _iEvt % 1000 == 0 && _doPreAlignment == 0)
	{
		streamlog_out( MESSAGE2 ) << "Processing event " << setw(6) << setiosflags(ios::right) << event->getEventNumber() << " in run " << setw(6) << setiosflags(ios::right) << event->getRunNumber() << ", with " << setw(6) << setiosflags(ios::right) << _nMilleTracks << " tracks, " << setw(6) << setiosflags(ios::right) << _nGoodMilleTracks << "  good" << endl;
	}

	if( _nMilleTracks > _maxTrackCandidatesTotal )
	{
		throw StopProcessingException(this);
	}

	EUTelEventImpl * evt = static_cast<EUTelEventImpl*> (event) ;
	if( evt->getEventType() == kEORE )
	{
		streamlog_out( DEBUG2 ) << "EORE found: nothing else to do." << endl;
		return;
	}

	std::vector<std::vector<EUTelMilleGBL::HitsInPlane> > _hitsArray(_nPlanes - _nExcludePlanes, std::vector<EUTelMilleGBL::HitsInPlane>() );
	std::vector<int> indexconverter (_nPlanes,-1);

	int icounter = 0;
	for( int i = 0; i < _nPlanes; i++ )
	{
		int excluded = 0; //0 - not excluded, 1 - excluded
		if( _nExcludePlanes > 0 )
		{
			for( int helphelp = 0; helphelp < _nExcludePlanes; helphelp++ )
			{
				if( i == _excludePlanes[helphelp] )
				{
					excluded = 1;
					break;//leave the for loop
				}
			}
		}
		if( excluded == 1 )
		{
			indexconverter[i] = -1;
		} else
		{
			// HACK
			if (i < 3)
			{
				indexconverter[i] = icounter;
				icounter++;
			}
			if (i==3)
			{
				indexconverter[i] = 6;
			}
			if (i==4)
			{
				indexconverter[i] = 3;
			}
			if (i==5)
			{
				indexconverter[i] = 4;
			}
			if (i==6)
			{
				indexconverter[i] = 5;
			}
			if (i==7 && _useREF == true)
			{
				indexconverter[i] = 7;
			}
		}
	}

	// mode 0: read hits, do track finding
	// mode 2: simulate, do track finding
	if( _inputMode == 0 || _inputMode == 2 )
	{

		for( size_t i = 0; i < _hitCollectionName.size(); i++ )
		{

			LCCollection* collection;
			try {
				collection = event->getCollection(_hitCollectionName[i]);
			} catch (DataNotAvailableException& e)
			{
				throw SkipEventException(this);
			}

			int layerIndex = -1;
			HitsInPlane hitsInPlane;

			// check if running in input mode 0 or 2
			if( _inputMode == 0 )
			{

				// loop over all hits in collection:
				for( int iHit = 0; iHit < collection->getNumberOfElements(); iHit++ )
				{

					TrackerHitImpl * hit = static_cast<TrackerHitImpl*> ( collection->getElementAt(iHit) );
					LCObjectVec clusterVector = hit->getRawHits();

					double minDistance =  numeric_limits< double >::max();
					double * hitPosition = const_cast<double * > (hit->getPosition());

					for(  int i = 0; i < (int)_siPlaneZPosition.size(); i++ )
					{
						// for the dut: add z shifts to the finding procedure
						double totalshift = 0.0;
						if (indexconverter[i] == _manualDUTid)
						{
							for (size_t j = 0; j< _pre_alignmentCollectionName.size(); j++)
							{
							    totalshift += _dut_pre_align_z[j];
							}
							for (size_t j = 0; j< _alignmentCollectionName.size(); j++)
							{
							    totalshift += _dut_align_z[j];
							}
						}
						double distance = std::abs( hitPosition[2] - _siPlaneZPosition[i] + totalshift );
						if( distance < minDistance )
						{
							minDistance = distance;
							layerIndex = i;
						}
					}

					if( minDistance > 300 ) /* mm */
					{

						// advise the user that the guessing wasn't successful
						streamlog_out( WARNING3 ) << "A hit was found " << minDistance << " mm far from the nearest plane\n"
						"Please check the consistency of the data with the GEAR file" << endl;
					}

					// Getting positions of the hits, conversion to um
					hitsInPlane.measuredX = 1000 * hit->getPosition()[0]; // um
					hitsInPlane.measuredY = 1000 * hit->getPosition()[1]; // um
					hitsInPlane.measuredZ = 1000 * hit->getPosition()[2]; // um

					//printf("hit %5d of %5d , at %-8.3f %-8.3f %-8.3f, %5d %5d \n", iHit , collection->getNumberOfElements(), hitsInPlane.measuredX*1E-3, hitsInPlane.measuredY*1E-3, hitsInPlane.measuredZ*1E-3, indexconverter[layerIndex], layerIndex );

					if( indexconverter[layerIndex] != -1 )
					{
						_hitsArray[indexconverter[layerIndex]].push_back( hitsInPlane );
					}

				} // end loop over all hits in collection

			} else if ( _inputMode == 2) // done mode 0, mode 2 means simulation
			{

				#if defined( USE_ROOT ) || defined(MARLIN_USE_ROOT)

				const float resolX = _testModeSensorResolution;
				const float resolY = _testModeSensorResolution;
				const float xhitpos = gRandom->Uniform(-3500.0,3500.0);
				const float yhitpos = gRandom->Uniform(-3500.0,3500.0);
				const float xslope = gRandom->Gaus(0.0,_testModeXTrackSlope);
				const float yslope = gRandom->Gaus(0.0,_testModeYTrackSlope);

				// loop over all planes
				for( int help = 0; help < _nPlanes; help++ )
				{

					// The x and y positions are given by the sums of the measured
					// hit positions, the detector resolution, the shifts of the
					// planes and the effect due to the track slopes.
					hitsInPlane.measuredX = xhitpos + gRandom->Gaus(0.0,resolX) + _testModeSensorXShifts[help] + _testModeSensorZPositions[help] * tan(xslope) - _testModeSensorGamma[help] * yhitpos - _testModeSensorBeta[help] * _testModeSensorZPositions[0];
					hitsInPlane.measuredY = yhitpos + gRandom->Gaus(0.0,resolY) + _testModeSensorYShifts[help] + _testModeSensorZPositions[help] * tan(yslope) + _testModeSensorGamma[help] * xhitpos - _testModeSensorAlpha[help] * _testModeSensorZPositions[help];
					hitsInPlane.measuredZ = _testModeSensorZPositions[help];

					if( indexconverter[help] != -1 )
					{
						_hitsArray[indexconverter[help]].push_back( hitsInPlane );
					}

					// ADDITIONAL PRINTOUTS   
					// printf("plane:%3d hit:%3d %8.3f %8.3f %8.3f \n", help, indexconverter[help], hitsInPlane.measuredX,hitsInPlane.measuredY,hitsInPlane.measuredZ);

					_hitsArray[help].push_back(hitsInPlane);

				} // end loop over all planes

				#else // USE_ROOT

					throw MissingLibraryException( this, "ROOT" );

				#endif

			} // end if check running in input mode 0 or 2

		} //loop over hits

	} // end if inputmode 0 or 2

	streamlog_out( DEBUG0 ) << "Hits per plane: ";
	for( size_t i = 0; i < _hitsArray.size(); i++ )
	{
		streamlog_out( DEBUG0 ) << _hitsArray[i].size() << "  ";
	}
	streamlog_out( DEBUG0 ) << endl;

	std::vector<int> fitplane(_nPlanes, 0);
	for( int help = 0; help < _nPlanes; help++ )
	{
		fitplane[help] = 1;
	}

	// DP: correlate telescope hits from different planes

	int iA = indexconverter[0]; // plane 0

	// not excluded
	if( iA >= 0 )
	{

		// hits in plane
		for( size_t jA = 0; jA < _hitsArray[iA].size(); jA++ )
		{

			// plane 1
			int iB = indexconverter[1];

			// not excluded
			if( iB >= 0 )
			{

				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx01Hist->fill( dx );
					dy01Hist->fill( dy );
					if (_dthistos == true)
					{
						dx01dtHist->fill( event->getEventNumber(), dx );
						dy01dtHist->fill( event->getEventNumber(), dy );
					}
				} //loop hits jB

			}//iB valid

			// plane 2
			iB = indexconverter[2];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx02Hist->fill( dx );
					dy02Hist->fill( dy );
				} //loop hits jB

			} //iB valid

			// plane 6 = DUT
			iB = indexconverter[3];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx06Hist->fill( dx );
					dy06Hist->fill( dy );
				} //loop hits jB

			} //iB valid

			// plane 3
			iB = indexconverter[4];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx03Hist->fill( dx );
					dy03Hist->fill( dy );
				} //loop hits jB

			} //iB valid

			// plane 4
			iB = indexconverter[5];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx04Hist->fill( dx );
					dy04Hist->fill( dy );
				} //loop hits jB

			} //iB valid

			// plane 5
			iB = indexconverter[6];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx05Hist->fill( dx );
					dy05Hist->fill( dy );
				} //loop hits jB

			}//iB valid

			// REF
			if (_useREF == true)
			{
				iB = indexconverter[7];

				// not excluded
				if( iB >= 0 )
				{
					for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
					{
						double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
						double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
						dx07Hist->fill( dx );
						dy07Hist->fill( dy );
				    } //loop hits jB

				}//iB valid
			} // use REF

		} //loop hits jA

	} // iA valid

	iA = indexconverter[1]; // plane 1

	// not excluded
	if( iA >= 0 && _doPreAlignment == 0)
	{

		// hits in plane
		for( size_t jA = 0; jA < _hitsArray[iA].size(); jA++ )
		{

			// plane 2
			int iB = indexconverter[2];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx12Hist->fill( dx );
					dy12Hist->fill( dy );
					if (_dthistos == true)
					{
						dx12dtHist->fill( event->getEventNumber(), dx );
						dy12dtHist->fill( event->getEventNumber(), dy );
					}
				} //loop hits jB

			} //iB valid

			// plane 6 = DUT
			iB = indexconverter[3];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx16Hist->fill( dx );
					dy16Hist->fill( dy );
				} //loop hits jB

			} //iB valid

			// plane 3
			iB = indexconverter[4];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx13Hist->fill( dx );
					dy13Hist->fill( dy );
				} //loop hits jB

			} //iB valid

			// plane 4
			iB = indexconverter[5];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx14Hist->fill( dx );
					dy14Hist->fill( dy );
				} //loop hits jB

			} //iB valid

			// plane 5
			iB = indexconverter[6];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx15Hist->fill( dx );
					dy15Hist->fill( dy );
				} //loop hits jB

			}//iB valid

			// REF
			if (_useREF == true)
			{
				iB = indexconverter[7];

				// not excluded
				if( iB >= 0 )
				{
					for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
					{
						double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
						double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
						dx17Hist->fill( dx );
						dy17Hist->fill( dy );
				    } //loop hits jB

				}//iB valid
			} // use REF

		} //loop hits jA

	} // iA valid

	iA = indexconverter[2]; // plane 2

	// not excluded
	if( iA >= 0 && _doPreAlignment == 0)
	{

		// hits in plane
		for( size_t jA = 0; jA < _hitsArray[iA].size(); jA++ )
		{

			// plane 6 = DUT
			int iB = indexconverter[3];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx26Hist->fill( dx );
					dy26Hist->fill( dy );
					if (_dthistos == true)
					{
						dx26dtHist->fill( event->getEventNumber(), dx );
						dy26dtHist->fill( event->getEventNumber(), dy );
					}
				} //loop hits jB

			} //iB valid

			// plane 3
			iB = indexconverter[4];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx23Hist->fill( dx );
					dy23Hist->fill( dy );
					if (_dthistos == true)
					{
						dx23dtHist->fill( event->getEventNumber(), dx );
						dy23dtHist->fill( event->getEventNumber(), dy );
					}
				} //loop hits jB

			} //iB valid

			// plane 4
			iB = indexconverter[5];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx24Hist->fill( dx );
					dy24Hist->fill( dy );
				} //loop hits jB

			} //iB valid

			// plane 5
			iB = indexconverter[6];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx25Hist->fill( dx );
					dy25Hist->fill( dy );
				} //loop hits jB

			}//iB valid

			// REF
			if (_useREF == true)
			{
				iB = indexconverter[7];

				// not excluded
				if( iB >= 0 )
				{
					for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
					{
						double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
						double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
						dx27Hist->fill( dx );
						dy27Hist->fill( dy );
				    } //loop hits jB

				}//iB valid
			} // use REF

		} //loop hits jA

	} // iA valid

	iA = indexconverter[4]; // plane 3

	// not excluded
	if( iA >= 0 && _doPreAlignment == 0)
	{

		// hits in plane
		for( size_t jA = 0; jA < _hitsArray[iA].size(); jA++ )
		{

			// plane 6 = DUT
			int iB = indexconverter[3];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx36Hist->fill( dx );
					dy36Hist->fill( dy );
				} //loop hits jB

			} //iB valid

			// plane 4
			iB = indexconverter[5];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx34Hist->fill( dx );
					dy34Hist->fill( dy );
					if (_dthistos == true)
					{
						dx34dtHist->fill( event->getEventNumber(), dx );
						dy34dtHist->fill( event->getEventNumber(), dy );
					}
				} //loop hits jB

			} //iB valid

			// plane 5
			iB = indexconverter[6];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx35Hist->fill( dx );
					dy35Hist->fill( dy );
				} //loop hits jB

			}//iB valid

			// REF
			if (_useREF == true)
			{
				iB = indexconverter[7];

				// not excluded
				if( iB >= 0 )
				{
					for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
					{
						double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
						double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
						dx37Hist->fill( dx );
						dy37Hist->fill( dy );
				    } //loop hits jB

				}//iB valid
			} // use REF

		} //loop hits jA

	} // iA valid

	iA = indexconverter[5]; // plane 4

	// not excluded
	if( iA >= 0 && _doPreAlignment == 0)
	{

		// hits in plane
		for( size_t jA = 0; jA < _hitsArray[iA].size(); jA++ )
		{

			// plane 6 = DUT
			int iB = indexconverter[3];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx46Hist->fill( dx );
					dy46Hist->fill( dy );
				} //loop hits jB

			} //iB valid

			// plane 5
			iB = indexconverter[6];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx45Hist->fill( dx );
					dy45Hist->fill( dy );
					if (_dthistos == true)
					{
						dx45dtHist->fill( event->getEventNumber(), dx );
						dy45dtHist->fill( event->getEventNumber(), dy );
					}
				} //loop hits jB

			}//iB valid

			// REF
			if (_useREF == true)
			{
				iB = indexconverter[7];

				// not excluded
				if( iB >= 0 )
				{
					for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
					{
						double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
						double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
						dx47Hist->fill( dx );
						dy47Hist->fill( dy );
				    } //loop hits jB

				}//iB valid
			} // use REF

		} //loop hits jA

	} // iA valid

	iA = indexconverter[6]; // plane 5

	// not excluded
	if( iA >= 0 && _doPreAlignment == 0)
	{

		// hits in plane
		for( size_t jA = 0; jA < _hitsArray[iA].size(); jA++ )
		{

			// plane 6 = DUT
			int iB = indexconverter[3];

			// not excluded
			if( iB >= 0 )
			{
				for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
				{
					double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
					double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
					dx56Hist->fill( dx );
					dy56Hist->fill( dy );
				} //loop hits jB

			} //iB valid

			// REF
			if (_useREF == true)
			{
				iB = indexconverter[7];

				// not excluded
				if( iB >= 0 )
				{
					for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
					{
						double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
						double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
						dx57Hist->fill( dx );
						dy57Hist->fill( dy );
						if (_dthistos == true)
						{
							dx57dtHist->fill( event->getEventNumber(), dx );
							dy57dtHist->fill( event->getEventNumber(), dy );
						}
				    } //loop hits jB

				}//iB valid
			} // use REF

		} //loop hits jA

	} // iA valid
	
	if (_useREF == true)
	{

		iA = indexconverter[3]; // DUT

		// not excluded
		if( iA >= 0 && _doPreAlignment == 0)
		{

			// hits in plane
			for( size_t jA = 0; jA < _hitsArray[iA].size(); jA++ )
			{

				int iB = indexconverter[7];

				// not excluded
				if( iB >= 0 )
				{
					for( size_t jB = 0; jB < _hitsArray[iB].size(); jB++ )
					{
						double dx = _hitsArray[iB][jB].measuredX - _hitsArray[iA][jA].measuredX;
						double dy = _hitsArray[iB][jB].measuredY - _hitsArray[iA][jA].measuredY;
						dx67Hist->fill( dx );
						dy67Hist->fill( dy );
				    } //loop hits jB

				}//iB valid

			} //loop hits jA

		} // iA valid
	}

	// triplets 0-1-2-6:
	// driplets 3-4-5-(7):

	// i is plane index
	// j is hit index
	// k is triplet index

	int i0 = indexconverter[0]; // plane 0
	int i1 = indexconverter[1]; // plane 1
	int i2 = indexconverter[2]; // plane 2
	int i6 = indexconverter[3]; // DUT plane 6

	int i3 = indexconverter[4]; // plane 3
	int i4 = indexconverter[5]; // plane 4
	int i5 = indexconverter[6]; // plane 5
	int i7 = -1;
	if (_useREF == true)
	{
		i7 = indexconverter[7]; // REF
	}


	// no plane excluded
	if( i0*i1*i2*i3*i4*i5*i6 >= 0 && _doPreAlignment == 0)
	{

		int ntri = 0;
		double xmA[99];
		double ymA[99];
		double zmA[99];
		double sxA[99];
		double syA[99];
		double szA[99];
		double trxA[99];
		double tryA[99];
		double trzA[99];
		double pxA[99];
		double pyA[99];
		double pzA[99];
		int hts[8][99]; // 6+2 planes

		// hits in plane
		for( size_t j0 = 0; j0 < _hitsArray[i0].size(); j0++ )
		{

			for( size_t j2 = 0; j2 < _hitsArray[i2].size(); j2++ )
			{

				double dx02 = _hitsArray[i2][j2].measuredX - _hitsArray[i0][j0].measuredX;
				double dy02 = _hitsArray[i2][j2].measuredY - _hitsArray[i0][j0].measuredY;
				double dz02 = _hitsArray[i2][j2].measuredZ - _hitsArray[i0][j0].measuredZ;

				double avx = 0.5 * ( _hitsArray[i0][j0].measuredX + _hitsArray[i2][j2].measuredX ); // average
				double avy = 0.5 * ( _hitsArray[i0][j0].measuredY + _hitsArray[i2][j2].measuredY ); // average
				double avz = 0.5 * ( _hitsArray[i0][j0].measuredZ + _hitsArray[i2][j2].measuredZ ); // average

				double tx = dx02 / dz02; // angle theta x
				double ty = dy02 / dz02; // angle theta y
				double tz = 1.0; // angle theta y

				// middle plane 1 for triplets:

				for( size_t j1 = 0; j1 < _hitsArray[i1].size(); j1++ )
				{

					// triplet residual:
					double zs = _hitsArray[i1][j1].measuredZ - avz;
					double xs = avx + tx * zs; // track at 1
					double ys = avy + ty * zs;

					double dx = _hitsArray[i1][j1].measuredX - xs;
					double dy = _hitsArray[i1][j1].measuredY - ys;

					// require DUT hit?
					if (_requireDUTHit == 1)
					{

						for( size_t j6 = 0; j6 < _hitsArray[i6].size(); j6++ )
						{

							// triplet residual to DUT:
							double zs6 = _hitsArray[i6][j6].measuredZ - _hitsArray[i0][j0].measuredZ;
							double xs6 = dx02 / dz02 * zs6 + _hitsArray[i0][j0].measuredX;
							double ys6 = dy02 / dz02 * zs6 + _hitsArray[i0][j0].measuredY;

							double dx6 = _hitsArray[i6][j6].measuredX - xs6;
							double dy6 = _hitsArray[i6][j6].measuredY - ys6;
							double dz6 = _hitsArray[i6][j6].measuredZ - zs6;

							streamlog_out (DEBUG1) << "DUT hit: x: " <<  _hitsArray[i6][j6].measuredX << " y: " << _hitsArray[i6][j6].measuredY << " z: " << _hitsArray[i6][j6].measuredZ << endl;
							streamlog_out (DEBUG1) << "DUT triplet residual x: " << dx6 << endl;
							streamlog_out (DEBUG1) << "DUT triplet residual y: " << dy6 << endl;
							streamlog_out (DEBUG1) << "DUT triplet residual z: " << dz6 << endl;

							/* use this for debug...
							// check if hit is in the DUT plane
							TVector3 theduthit(_hitsArray[i6][j6].measuredX,_hitsArray[i6][j6].measuredY,_hitsArray[i6][j6].measuredZ);
							bool hitquality = isPointInPlane(theduthit);
							if (hitquality)
							{
								//streamlog_out (MESSAGE4) << "DUT hit in plane!" << endl;
							} else {
							    	streamlog_out (MESSAGE4) << "DUT hit NOT in plane!" << endl;
							}
							*/

							if( abs(dx) < _triCut )
							{
								trirxHist->fill( dx );
							}
							if( abs(dx6) < _triCutDUTx )
							{
								trirxdutHist->fill( dx6 );
							}

							if( abs(dy) < _triCut )
							{
								triryHist->fill( dy );
							}
							if( abs(dy6) < _triCutDUTy )
							{
								trirydutHist->fill( dy6 );
							}

							if( abs(dx) < _triCut  && abs(dy) < _triCut && abs(dx6) < _triCutDUTx && abs(dy6) < _triCutDUTy)
							{

								streamlog_out (DEBUG2) << "Passed all triplet cuts, now having "<< ntri << " triplets." << endl;

								if( ntri < 99 )
								{

									xmA[ntri] = avx;
									ymA[ntri] = avy;
									zmA[ntri] = avz;
									sxA[ntri] = tx;
									syA[ntri] = ty;
									szA[ntri] = tz;
									trxA[ntri] = dx02;
									tryA[ntri] = dy02;
									trzA[ntri] = dz02;
									pxA[ntri] = _hitsArray[i2][j2].measuredX;
									pyA[ntri] = _hitsArray[i2][j2].measuredY;
									pzA[ntri] = _hitsArray[i2][j2].measuredZ;
									hts[indexconverter[0]][ntri] = j0;
									hts[indexconverter[1]][ntri] = j1;
									hts[indexconverter[2]][ntri] = j2;
									hts[indexconverter[3]][ntri] = j6;
								}
								ntri++;

							} //valid triplet

						} // loop hits j6

					// if DUT hit required
					} else if (_requireDUTHit == 0)
					{

						if( abs(dx) < _triCut )
						{
							trirxHist->fill( dx );
						}
						if( abs(dy) < _triCut )
						{
							triryHist->fill( dy );
						}

						if( abs(dx) < _triCut  && abs(dy) < _triCut )
						{

							streamlog_out (DEBUG2) << "Passed all triplet cuts, now having "<< ntri << " triplets." << endl;

							if( ntri < 99 )
							{

								xmA[ntri] = avx;
								ymA[ntri] = avy;
								zmA[ntri] = avz;
								sxA[ntri] = tx;
								syA[ntri] = ty;
								szA[ntri] = tz;
								trxA[ntri] = dx02;
								tryA[ntri] = dy02;
								trzA[ntri] = dz02;
								pxA[ntri] = _hitsArray[i2][j2].measuredX;
								pyA[ntri] = _hitsArray[i2][j2].measuredY;
								pzA[ntri] = _hitsArray[i2][j2].measuredZ;
								hts[indexconverter[0]][ntri] = j0;
								hts[indexconverter[1]][ntri] = j1;
								hts[indexconverter[2]][ntri] = j2;

							}
							ntri++;

						} //valid triplet

					// no DUT hit required 
					} else 
					{
						streamlog_out (ERROR2) << "Unknown setting for RequireDUTHit: " << _requireDUTHit << endl;
					}

				} //loop hits j1

			} //loop hits j2

		} //loop hits j0

		ntriHist->fill( ntri );

		if( ntri >= 99 )
		{
			streamlog_out( WARNING2 ) << "Maximum number of triplet track candidates reached in event " << event->getEventNumber() << ". Maybe further tracks were skipped" << endl;
		}

		// driplets 3-4-5:

		int ndri = 0;
		double xmB[99];
		double ymB[99];
		double zmB[99];
		double sxB[99];
		double syB[99];
		double trxB[99];
		double tryB[99];
		double trzB[99];
		double pxB[99];
		double pyB[99];
		double pzB[99];

		// hits in plane
		for( size_t j3 = 0; j3 < _hitsArray[i3].size(); j3++ )
		{

			for( size_t j5 = 0; j5 < _hitsArray[i5].size(); j5++ )
			{

				double dx35 = _hitsArray[i5][j5].measuredX - _hitsArray[i3][j3].measuredX;
				double dy35 = _hitsArray[i5][j5].measuredY - _hitsArray[i3][j3].measuredY;
				double dz35 = _hitsArray[i5][j5].measuredZ - _hitsArray[i3][j3].measuredZ;

				double avx = 0.5 * ( _hitsArray[i3][j3].measuredX + _hitsArray[i5][j5].measuredX ); // average
				double avy = 0.5 * ( _hitsArray[i3][j3].measuredY + _hitsArray[i5][j5].measuredY ); // average
				double avz = 0.5 * ( _hitsArray[i3][j3].measuredZ + _hitsArray[i5][j5].measuredZ ); // average

				double tx = dx35 / dz35; // angle theta x
				double ty = dy35 / dz35; // angle theta x

				// middle plane 4 for driplets:

				for( size_t j4 = 0; j4 < _hitsArray[i4].size(); j4++ )
				{

					// driplet residual:

					double zs = _hitsArray[i4][j4].measuredZ - avz;
					double xs = avx + tx * zs; // track at 4
					double ys = avy + ty * zs;

					double dx = _hitsArray[i4][j4].measuredX - xs;
					double dy = _hitsArray[i4][j4].measuredY - ys;
					
					if (_useREF == false)
					{

						if( abs(dx) < _driCut )
						{
							drirxHist->fill( dx );
						}
						if( abs(dy) < _driCut )
						{
							driryHist->fill( dy );
						}

						if( abs(dx) < _driCut  && abs(dy) < _driCut )
						{

							streamlog_out (DEBUG2) << "Passed all driplet cuts, now having "<< ndri << " driplets." << endl;

							if( ndri < 99 )
							{
								xmB[ndri] = avx;
								ymB[ndri] = avy;
								zmB[ndri] = avz;
								sxB[ndri] = tx;
								syB[ndri] = ty;
								trxB[ndri] = dx35;
								tryB[ndri] = dy35;
								trzB[ndri] = dz35;
								pxB[ndri] = _hitsArray[i5][j5].measuredX;
								pyB[ndri] = _hitsArray[i5][j5].measuredY;
								pzB[ndri] = _hitsArray[i5][j5].measuredZ;
								hts[indexconverter[4]][ndri] = j3;
								hts[indexconverter[5]][ndri] = j4;
								hts[indexconverter[6]][ndri] = j5;
							}

							ndri++;

						}//valid driplet

					} // without REF
					if (_useREF == true)
					{
						for( size_t j7 = 0; j7 < _hitsArray[i7].size(); j7++ )
						{

							// triplet residual to REF:
							double zs7 = _hitsArray[i7][j7].measuredZ - _hitsArray[i3][j3].measuredZ;
							double xs7 = dx35 / dz35 * zs7 + _hitsArray[i3][j3].measuredX;
							double ys7 = dy35 / dz35 * zs7 + _hitsArray[i3][j3].measuredY;

							double dx7 = _hitsArray[i7][j7].measuredX - xs7;
							double dy7 = _hitsArray[i7][j7].measuredY - ys7;
							double dz7 = _hitsArray[i7][j7].measuredZ - zs7;

							streamlog_out (DEBUG1) << "REF hit: x: " <<  _hitsArray[i7][j7].measuredX << " y: " << _hitsArray[i7][j7].measuredY << " z: " << _hitsArray[i7][j7].measuredZ << endl;
							streamlog_out (DEBUG1) << "REF driplet residual x: " << dx7 << endl;
							streamlog_out (DEBUG1) << "REF driplet residual y: " << dy7 << endl;
							streamlog_out (DEBUG1) << "REF driplet residual z: " << dz7 << endl;

							if( abs(dx) < _driCut )
							{
								drirxHist->fill( dx );
							}
							if( abs(dx7) < _driCutREFx )
							{
								drirxrefHist->fill( dx7 );
							}

							if( abs(dy) < _driCut )
							{
								driryHist->fill( dy );
							}
							if( abs(dy7) < _driCutREFy )
							{
								driryrefHist->fill( dy7 );
							}

							if( abs(dx) < _driCut  && abs(dy) < _driCut && abs(dx7) < _driCutREFx && abs(dy7) < _driCutREFy)
							{

								streamlog_out (DEBUG2) << "Passed all driplet cuts, now having "<< ndri << " driplets." << endl;

								if( ndri < 99 )
								{

									xmB[ndri] = avx;
									ymB[ndri] = avy;
									zmB[ndri] = avz;
									sxB[ndri] = tx;
									syB[ndri] = ty;
									trxB[ndri] = dx35;
									tryB[ndri] = dy35;
									trzB[ndri] = dz35;
									pxB[ndri] = _hitsArray[i5][j5].measuredX;
									pyB[ndri] = _hitsArray[i5][j5].measuredY;
									pzB[ndri] = _hitsArray[i5][j5].measuredZ;
									hts[indexconverter[4]][ndri] = j3;
									hts[indexconverter[5]][ndri] = j4;
									hts[indexconverter[6]][ndri] = j5;
									hts[indexconverter[7]][ndri] = j7;
								}
								ndri++;

							} //valid driplet

						} // loop hits j7
					} // use REF

				}//loop hits j4

			}//loop hits j5

		}//loop hits j3

		ndriHist->fill( ndri );

		if( ndri >= 99 )
		{
			streamlog_out( WARNING2 ) << "Maximum number of driplet track candidates reached in event " << event->getEventNumber() << ". Maybe further tracks were skipped" << endl;
		}

		// match triplets A to driplets B at DUT:

		int nm = 0;
		int nmGood = 0;

		if ( ntri > 0 && ndri > 0)
		{
			streamlog_out (DEBUG3) << "Matching "<< ntri << " triplets and " << ndri << " driplets." << endl;
		}

		// i = A
		for( int kA = 0; kA < ntri && kA < 99; ++kA )
		{

			int j2 = hts[indexconverter[2]][kA];

			// j = B
			for( int kB = 0; kB < ndri && kB < 99; ++kB )
			{

				int j3 = hts[indexconverter[4]][kB];

				// this is essentially the DUT position
				double zmid = 0.5 * ( _hitsArray[i2][j2].measuredZ + _hitsArray[i3][j3].measuredZ );

				double xA = xmA[kA] + sxA[kA] * ( zmid - zmA[kA] ); // A at zmid
				double yA = ymA[kA] + syA[kA] * ( zmid - zmA[kA] );
				double zA = zmA[kA] + szA[kA] * ( zmid - zmA[kA] );

				double xB = xmB[kB] + sxB[kB] * ( zmid - zmB[kB] ); // B at zmid
				double yB = ymB[kB] + syB[kB] * ( zmid - zmB[kB] );

				double dx = xB - xA; // matching residual
				double dy = yB - yA;

				if( abs(dx) < _sixCut )
				{
					sixrxHist->fill( dx );
				}
				if( abs(dy) < _sixCut )
				{
					sixryHist->fill( dy );
				}

				// triplet-driplet match
				if( abs(dx) < _sixCut  && abs(dy) < _sixCut )
				{

					streamlog_out (DEBUG3) << "Found matching tri-driplet!" << endl;

					nm++;

					// calculate the DUT fit position, 3 principle methods:
					// 0: geometrical centre of the telescope (bad if DUT is rotated)
					// 1: extrapolate from triplet and get intersect with plane
					// 2: extrapolate from driplet and get intersect with plane
					// 3: get average from methods 1 and 2

					double dutfitposX = 0.0;
					double dutfitposY = 0.0;
					double dutfitposZ = 0.0;

					double scaA = 0.0;
					double scaB = 0.0;

					scaA = ( (_dutplane.X() - pxA[kA]) * _normalvector.X() + (_dutplane.Y() - pyA[kA]) * _normalvector.Y() + (_dutplane.Z() - pzA[kA]) * _normalvector.Z() ) / ( trxA[kA] * _normalvector.X() + tryA[kA] * _normalvector.Y() + trzA[kA] * _normalvector.Z() );

					double scxA = pxA[kA] + scaA * trxA[kA];
					double scyA = pyA[kA] + scaA * tryA[kA];
					double sczA = pzA[kA] + scaA * trzA[kA];

					scaB = ( (_dutplane.X() - pxB[kB]) * _normalvector.X() + (_dutplane.Y() - pyB[kB]) * _normalvector.Y() + (_dutplane.Z() - pzB[kB]) * _normalvector.Z() ) / ( trxB[kB] * _normalvector.X() + tryB[kB] * _normalvector.Y() + trzB[kB] * _normalvector.Z() );

					double scxB = pxB[kB] + scaB * trxB[kB];
					double scyB = pyB[kB] + scaB * tryB[kB];
					double sczB = pzB[kB] + scaB * trzB[kB];

					if (_dutFitMethod == 0)
					{
						dutfitposX = xA;
						dutfitposY = yA;
						dutfitposZ = zA;
					} else if (_dutFitMethod == 1)
					{
						dutfitposX = scxA;
						dutfitposY = scyA;
						dutfitposZ = sczA;
					} else if (_dutFitMethod == 2)
					{
						dutfitposX = scxB;
						dutfitposY = scyB;
						dutfitposZ = sczB;
					} else if (_dutFitMethod == 3)
					{
						dutfitposX = (scxA+scxB)/2.0;
						dutfitposY = (scyA+scyB)/2.0;
						dutfitposZ = (sczA+sczB)/2.0;
					}

					/* use this for debug...
					TVector3 tempvec(dutfitposX,dutfitposY,dutfitposZ);
					bool atempbool = isPointInPlane(tempvec);
					if (atempbool)
					{
						//streamlog_out (MESSAGE4) << "Track fit in plane!" << endl;
					} else {
					    	streamlog_out (MESSAGE4) << "Track fit NOT in plane!" << endl;
					}
					*/

					// fill into histos
					tres01x->fill(xA-scxA);
					tres01y->fill(yA-scyA);
					tres01z->fill(zA-sczA);

					tres02x->fill(xA-scxB);
					tres02y->fill(yA-scyB);
					tres02z->fill(zA-sczB);

					tres12x->fill(scxA-scxB);
					tres12y->fill(scyA-scyB);
					tres12z->fill(sczA-sczB);


					double kx = sxB[kB] - sxA[kA]; //kink
					double ky = syB[kB] - syA[kA];

					sixkxHist->fill( kx*1E3 );
					sixkyHist->fill( ky*1E3 );

					// GBL with triplet A as seed:

					// GBL point vector for the trajectory
					std::vector<gbl::GblPoint> traj_points;

					// gbl::GblTrajectory traj( false ); // curvature = false

					// build up trajectory:

					std::vector<unsigned int> ilab; // 0-5 = telescope, 6 = DUT
					vector<double> sPoint;

					double s = 0;

					TMatrixD jacPointToPoint( 5, 5 );

					TMatrixD proL2m(2,2);
					proL2m.UnitMatrix();

					TVectorD meas(2);

					TVectorD measPrec(2); // precision = 1/resolution^2
					measPrec[0] = 1.0 / _resx / _resx;
					measPrec[1] = 1.0 / _resy / _resy;

					// different meas precision for dut
					TVectorD measPrecDut(2); // precision = 1/resolution^2
					measPrecDut[0] = 1.0 / _resdutx / _resdutx;
					measPrecDut[1] = 1.0 / _resduty / _resduty;

					// different meas precision for ref
					TVectorD measPrecRef(2); // precision = 1/resolution^2
					measPrecRef[0] = 1.0 / _resrefx / _resrefx;
					measPrecRef[1] = 1.0 / _resrefy / _resrefy;

					// scatter:
					TVectorD scat(2);
					scat.Zero(); //mean is zero

					// FIXME this could be read from gear...
					double p = _eBeam; // beam momentum
					double X0Si = 65e-3 / 94.0; // Si + Kapton
					double tetSi = 0.0136 * sqrt(X0Si) / p * ( 1 + 0.038*std::log(X0Si) );
					double X0SiDUT = 350e-3 / 94.0; // Si + Kapton
					X0SiDUT = 0.048;
					double tetSiDUT = 0.0136 * sqrt(X0SiDUT) / p * ( 1 + 0.038*std::log(X0SiDUT) );
					double X0Air = 1.0 / 304200.0;

					TVectorD wscatSi(2);
					wscatSi[0] = 1.0 / ( tetSi * tetSi ); //weight
					wscatSi[1] = 1.0 / ( tetSi * tetSi );

					TVectorD wscatSiDUT(2);
					wscatSiDUT[0] = 1.0 / ( tetSiDUT * tetSiDUT ); //weight
					wscatSiDUT[1] = 1.0 / ( tetSiDUT * tetSiDUT );

					TMatrixD alDer2( 2, 2 ); // alignment derivatives
					alDer2[0][0] = 1.0; // dx/dx GBL sign convetion
					alDer2[1][0] = 0.0; // dy/dx
					alDer2[0][1] = 0.0; // dx/dy
					alDer2[1][1] = 1.0; // dy/dy

					TMatrixD alDer3( 2, 3 ); // alignment derivatives
					alDer3[0][0] = 1.0; // dx/dx
					alDer3[1][0] = 0.0; // dy/dx
					alDer3[0][1] = 0.0; // dx/dy
					alDer3[1][1] = 1.0; // dy/dy

					TMatrixD alDer3D( 3, 3 ); // alignment derivatives
					alDer3D[0][0] = 1.0; // dx/dx
					alDer3D[1][0] = 0.0; // dy/dx
					alDer3D[2][0] = 0.0; // dz/dx
					alDer3D[0][1] = 0.0; // dx/dy
					alDer3D[1][1] = 1.0; // dy/dy
					alDer3D[2][1] = 0.0; // dz/dy
					alDer3D[0][2] = 0.0; // dx/dz
					alDer3D[1][2] = 0.0; // dy/dz
					alDer3D[2][2] = 1.0; // dz/dz

					TMatrixD alDer4( 2, 4 ); // alignment derivatives
					alDer4[0][0] = 1.0; // dx/dx
					alDer4[1][0] = 0.0; // dy/dx
					alDer4[0][1] = 0.0; // dx/dy
					alDer4[1][1] = 1.0; // dy/dy
					alDer4[0][3] = sxA[kA]; // dx/dz
					alDer4[1][3] = syA[kA]; // dy/dz

					TMatrixD alDer6( 3, 6 ); // alignment derivatives

					// telescope planes 0-5 + DUT + REF:
					double rx[8];
					double ry[8];
					double rz[8];

					int jhit = hts[0][kA];
					double xprev = _hitsArray[indexconverter[0]][jhit].measuredX; // first plane, including any pre-alignment
					double yprev = _hitsArray[indexconverter[0]][jhit].measuredY; // first plane, including any pre-alignment
					double zprev = _hitsArray[indexconverter[0]][jhit].measuredZ; // first plane, including any pre-alignment

					// plane loop
					int looplimit = 7;
					if (_useREF == true)
					{
						looplimit = 8;
					}
					for( int ipl = 0; ipl < looplimit; ++ipl )
					{

						// in this case just add scatterer and continue with the next plane
						if (_requireDUTHit == 0 && ipl == 3)
						{

							// give correct z from fit!
							//double zz = _siPlaneZPosition[ipl]*1000.0; //_hitsArray[indexconverter[ipl]][jhit].measuredZ; // [um]
							double xx = dutfitposX;
							double yy = dutfitposY;
							double zz = dutfitposZ;

							double stepx = xx - xprev;
							double stepy = yy - yprev;
							double step = zz - zprev;

							TVector3 direction(stepx,stepy,step);

							jacPointToPoint = Jac55( step , direction , _BField );
							gbl::GblPoint *point = new gbl::GblPoint( jacPointToPoint );
							point->addScatterer( scat, wscatSiDUT );

							xprev = xx;
							yprev = yy;
							zprev = zz;
							traj_points.push_back(*point);
							sPoint.push_back( s );
							delete point;

							streamlog_out (DEBUG3) << "Adding GBL scatterer at z: " << zz << " um in Event " << event->getEventNumber() << endl;

							// also add air
							// positions in um
							zz = _siPlaneZPosition[ipl]*1000.0 + (_siPlaneZPosition[ipl+1]*1000.0 - _siPlaneZPosition[ipl]*1000.0)*0.21;
							step = zz - zprev;
							direction[2]=step;
							jacPointToPoint = Jac55( step , direction , _BField );
							gbl::GblPoint *point1 = new gbl::GblPoint( jacPointToPoint );
							s += step;
							zprev = zz;
							TVectorD wscatAir(2);
							// as x0 is in mm, airlength has to be in mm too!
							double airlength = (0.5 * (_siPlaneZPosition[ipl+1] - _siPlaneZPosition[ipl]))/1000.0;
							double tetAir = 0.0136 * sqrt(X0Air*airlength) / p * ( 1 + 0.038*std::log(X0Air*airlength) );
							wscatAir[0] = 1.0 / ( tetAir * tetAir );
							wscatAir[1] = 1.0 / ( tetAir * tetAir );
							point1->addScatterer( scat, wscatAir );
							traj_points.push_back(*point1);
							sPoint.push_back( s );
							delete point1;
							streamlog_out (DEBUG3) << "Adding air scatterer at z: " << zz << " um in Event " << event->getEventNumber() << endl;

							zz = zprev + (_siPlaneZPosition[ipl+1]*1000.0 - _siPlaneZPosition[ipl]*1000.0)*0.58;
							step = zz - zprev;
							direction[2]=step;
							jacPointToPoint = Jac55( step , direction , _BField );
							gbl::GblPoint *point2 = new gbl::GblPoint( jacPointToPoint );
							s += step;
							zprev = zz;
							point2->addScatterer( scat, wscatAir );
							traj_points.push_back(*point2);
							sPoint.push_back( s );
							delete point2;
							streamlog_out (DEBUG3) << "Adding air scatterer at z: " << zz << " um in Event " << event->getEventNumber() << endl;

							continue;
						}

						// either triplet or driplet
						if( ipl < 4)
						{
							jhit = hts[indexconverter[ipl]][kA];
						} else 
						{
							jhit = hts[indexconverter[ipl]][kB];
						}

						double xx = _hitsArray[indexconverter[ipl]][jhit].measuredX; // [um]
						double yy = _hitsArray[indexconverter[ipl]][jhit].measuredY; // [um]
						double zz = _hitsArray[indexconverter[ipl]][jhit].measuredZ; // [um]

						double stepx = xx - xprev;
						double stepy = yy - yprev;
						double step = zz - zprev;

						TVector3 direction(stepx,stepy,step);

						jacPointToPoint = Jac55( step , direction , _BField );
						gbl::GblPoint *point = new gbl::GblPoint( jacPointToPoint );
						s += step;
						xprev = xx;
						yprev = yy;
						zprev = zz;

						// extrapolate t/driplet vector A/B to each plane:
						double dz = 0.0;
						double xs = 0.0; // Ax at plane
						double ys = 0.0; // Ay at plane

						if( ipl < 4)
						{
							dz = zz - zmA[kA];
							xs = xmA[kA] + sxA[kA] * dz;
							ys = ymA[kA] + syA[kA] * dz;
						} else
						{
							dz = zz - zmB[kB];
							xs = xmB[kB] + sxB[kB] * dz;
							ys = ymB[kB] + syB[kB] * dz;
						}

						rx[indexconverter[ipl]] = _hitsArray[indexconverter[ipl]][jhit].measuredX - xs; // resid hit-track
						ry[indexconverter[ipl]] = _hitsArray[indexconverter[ipl]][jhit].measuredY - ys; // resid

						meas[0] = _hitsArray[indexconverter[ipl]][jhit].measuredX - xs; // resid hit-track
						meas[1] = _hitsArray[indexconverter[ipl]][jhit].measuredY - ys;

						// catch DUT, this has a different meas precision and scattering properties
						if (ipl == 3)
						{
							// measurement has correct z anyway
							point->addMeasurement( proL2m, meas, measPrecDut );
							point->addScatterer( scat, wscatSiDUT ); //

						} else if (ipl == 7)
						{
							point->addMeasurement( proL2m, meas, measPrecRef );
							point->addScatterer( scat, wscatSiDUT ); // don't care about scat...
						} else // assume all telescope planes are equal
						{
							point->addMeasurement( proL2m, meas, measPrec );
							point->addScatterer( scat, wscatSi );
						}

						streamlog_out (DEBUG3) << "Adding GBL measurement: x: " << _hitsArray[indexconverter[ipl]][jhit].measuredX << " y: " << _hitsArray[indexconverter[ipl]][jhit].measuredY << " z: " << _hitsArray[indexconverter[ipl]][jhit].measuredZ << " in Event " << event->getEventNumber() << endl;

						// FIXME may not work for 7?
						// only x and y shifts
						if( _alignMode == 2)
						{
							// global labels for MP:
							std::vector<int> globalLabels(2);
							globalLabels[0] = 1 + 2*ipl;
							globalLabels[1] = 2 + 2*ipl;
							// for MillePede alignment
							point->addGlobals( globalLabels, alDer2 );

						// with rot
						} else if ( _alignMode == 3 )
						{
							std::vector<int> globalLabels(3);
							globalLabels[0] = 1 + 3*ipl; // x
							globalLabels[1] = 2 + 3*ipl; // y
							globalLabels[2] = 3 + 3*ipl; // rot
							//alDer3[0][2] = -_hitsArray[ipl][jhit].measuredY; // dx/dphi
							//alDer3[1][2] =  _hitsArray[ipl][jhit].measuredX; // dy/dphi
							alDer3[0][2] = -ys; // dx/dphi
							alDer3[1][2] =  xs; // dy/dphi
							// for MillePede alignment
							point->addGlobals( globalLabels, alDer3 );

						// with x y z shift
						}else if ( _alignMode == 7 )
						{
							std::vector<int> globalLabels(3);
							globalLabels[0] = 1 + 3*ipl; // x
							globalLabels[1] = 2 + 3*ipl; // y
							globalLabels[2] = 3 + 3*ipl; // z
							// for MillePede alignment
							point->addGlobals( globalLabels, alDer3D );

						// with rot and z shift
						} else if( _alignMode == 4 )
						{
							std::vector<int> globalLabels(4);
							globalLabels[0] = 1 + 4*ipl;
							globalLabels[1] = 2 + 4*ipl;
							globalLabels[2] = 3 + 4*ipl;
							globalLabels[3] = 4 + 4*ipl; // z
							//alDer4[0][2] = -_hitsArray[ipl][jhit].measuredY; // dx/dphi
							//alDer4[1][2] =  _hitsArray[ipl][jhit].measuredX; // dy/dphi
							alDer4[0][2] = -ys; // dx/dphi
							alDer4[1][2] =  xs; // dy/dphi
							point->addGlobals( globalLabels, alDer4 ); // for MillePede alignment

						// with multiple rot and/or z fixed
						} else if( _alignMode == 5 || _alignMode == 6 || _alignMode == 1)
						{
							std::vector<int> globalLabels(6);
							globalLabels[0] = 1 + 6*ipl;
							globalLabels[1] = 2 + 6*ipl;
							globalLabels[2] = 3 + 6*ipl;
							globalLabels[3] = 4 + 6*ipl;
							globalLabels[4] = 5 + 6*ipl;
							globalLabels[5] = 6 + 6*ipl;

							// sanity output
							streamlog_out (DEBUG0) << "Measured z of ipl " << ipl << " is " << _hitsArray[indexconverter[ipl]][jhit].measuredZ << " um" << endl;
							streamlog_out (DEBUG0) << "Gearfile z of ipl " << ipl << " is " << _siPlaneZPosition[ipl]*1000.0 << " um" << endl;
							double deltaz = _hitsArray[indexconverter[ipl]][jhit].measuredZ - _siPlaneZPosition[ipl]*1000.0;
							streamlog_out (DEBUG0) << "GBL DeltaZ of ipl " << ipl << " is " << deltaz << " um" << endl;

							// deltaz cannot be zero, otherwise this mode doesn't work
							// 1e-6 in um is quite small
							if (deltaz < 1e-6)
							{
								deltaz = 1e-6;
							}

							// derivatives:
							alDer6[0][0] = 1.0;		// dx/dx
							alDer6[0][1] = 0.0;		// dx/dy
							if( ipl < 4)
							{
								alDer6[0][2] = sxA[kA];	// dx/dz
							} else {
								alDer6[0][2] = sxB[kB];	// dx/dz
							}
							alDer6[0][3] = 0.0;		// dx/da
							alDer6[0][4] = deltaz;		// dx/db
							alDer6[0][5] = -ys;		// dx/dg

							alDer6[1][0] = 0.0;		// dy/dx
							alDer6[1][1] = 1.0;		// dy/dy
							if( ipl < 4)
							{
								alDer6[1][2] = syA[kA];	// dy/dz
							} else {
								alDer6[1][2] = syB[kB];	// dy/dz
							}
							alDer6[1][3] = -deltaz;		// dy/da
							alDer6[1][4] = 0.0;		// dy/db
							alDer6[1][5] = xs;		// dy/dg

							alDer6[2][0] = 0.0;		// dz/dx
							alDer6[2][1] = 0.0;		// dz/dy
							alDer6[2][2] = 1.0;		// dz/dz
							alDer6[2][3] = ys;		// dz/da
							alDer6[2][4] = -xs;		// dz/db
							alDer6[2][5] = 0.0;		// dz/dg

							/*
							// from eutelmille *-1 // FIXME crashes sometimes?!
							alDer6[0][0] = 1.0;					// dx/dx
							alDer6[0][1] = 0.0;					// dx/dy
							alDer6[0][2] = 0.0;					// dx/dz
							alDer6[0][3] = 0.0;					// dx/da
							alDer6[0][4] = deltaz;					// dx/db
							alDer6[0][5] = -1.0*_hitsArray[ipl][jhit].measuredY;	// dx/dg

							alDer6[1][0] = 0.0;	// dy/dx
							alDer6[1][1] = 1.0;	// dy/dy
							alDer6[1][2] = 0.0;	// dy/dz
							alDer6[1][3] = -1.0*deltaz;	// dy/da
							alDer6[1][4] = 0.0;	// dy/db
							alDer6[1][5] = 1.0*_hitsArray[ipl][jhit].measuredX;	// dy/dg

							alDer6[2][0] = 0.0;	// dz/dx
							alDer6[2][1] = 0.0; 	// dz/dy
							alDer6[2][2] = 1.0;	// dz/dz
							alDer6[2][3] = 1.0*_hitsArray[ipl][jhit].measuredX;	// dz/da
							alDer6[2][4] = -1.0*_hitsArray[ipl][jhit].measuredY;	// dz/db
							alDer6[2][5] = 0.0;	// dz/dg
							*/

							point->addGlobals( globalLabels, alDer6 ); // for MillePede alignment
						}

						//unsigned int iLabel = traj.addPoint(*point);
						traj_points.push_back(*point);

						//ilab[ipl] = iLabel;
						sPoint.push_back( s );

						streamlog_out (DEBUG2) << "Point pushed to gbl:" << endl; 
						streamlog_out (DEBUG2) << " Labels:" << endl;
						for (size_t ic=0;ic<point->getGlobalLabels().size();ic++)
						{
							streamlog_out (DEBUG2) << "  " << ic << " " << point->getGlobalLabels().at(ic) << endl;
						}
						streamlog_out(DEBUG2)<<" The number of global parameters for this point is "<<point->getNumGlobals()<< endl;
						streamlog_out(DEBUG2)<<"The alignment matrix after adding the point: " <<endl;
						streamlog_message( DEBUG2, point->getGlobalDerivatives().Print() ;, std::endl; );

						delete point;

						// add air after each plane except last one
						if (ipl < (_nPlanes - 1))
						{
							// positions in um
							zz = _siPlaneZPosition[ipl]*1000.0 + (_siPlaneZPosition[ipl+1]*1000.0 - _siPlaneZPosition[ipl]*1000.0)*0.21;
							step = zz - zprev;
							direction[2]=step;
							jacPointToPoint = Jac55( step , direction , _BField );
							gbl::GblPoint *point1 = new gbl::GblPoint( jacPointToPoint );
							s += step;
							zprev = zz;
							TVectorD wscatAir(2);
							// as x0 is in mm, airlength has to be in mm too!
							double airlength = (0.5 * (_siPlaneZPosition[ipl+1] - _siPlaneZPosition[ipl]))/1000.0;
							double tetAir = 0.0136 * sqrt(X0Air*airlength) / p * ( 1 + 0.038*std::log(X0Air*airlength) );
							wscatAir[0] = 1.0 / ( tetAir * tetAir );
							wscatAir[1] = 1.0 / ( tetAir * tetAir );
							point1->addScatterer( scat, wscatAir );
							traj_points.push_back(*point1);
							sPoint.push_back( s );
							delete point1;
							streamlog_out (DEBUG3) << "Adding air scatterer at z: " << zz << " um in Event " << event->getEventNumber() << endl;

							zz = zprev + (_siPlaneZPosition[ipl+1]*1000.0 - _siPlaneZPosition[ipl]*1000.0)*0.58;
							step = zz - zprev;
							direction[2]=step;
							jacPointToPoint = Jac55( step , direction , _BField );
							gbl::GblPoint *point2 = new gbl::GblPoint( jacPointToPoint );
							s += step;
							zprev = zz;
							point2->addScatterer( scat, wscatAir );
							traj_points.push_back(*point2);
							sPoint.push_back( s );
							delete point2;
							streamlog_out (DEBUG3) << "Adding air scatterer at z: " << zz << " um in Event " << event->getEventNumber() << endl;
						} // not last plane

					} // loop over planes

					// monitor what we have put into GBL:

					selxtHist->fill( xA*1E-3 );	// triplet at telescope middle
					selytHist->fill( yA*1E-3 );
					selaxtHist->fill( sxA[kA]*1E3 );	// track slope
					selaytHist->fill( syA[kA]*1E3 );

					selxdHist->fill( xB*1E-3 );	// driplet at telescope middle
					selydHist->fill( yB*1E-3 );
					selaxdHist->fill( sxB[kB]*1E3 );	// track slope
					selaydHist->fill( syB[kB]*1E3 );

					selrxHist->fill( dx );		// triplet-driplet match
					selryHist->fill( dy );
					selkxHist->fill( kx*1E3 );	// triplet-driplet kink
					selkyHist->fill( ky*1E3 );

					selrx0Hist->fill( rx[indexconverter[0]] );
					selry0Hist->fill( ry[indexconverter[0]] );
					selrx1Hist->fill( rx[indexconverter[1]] );
					selry1Hist->fill( ry[indexconverter[1]] );
					selrx2Hist->fill( rx[indexconverter[2]] );
					selry2Hist->fill( ry[indexconverter[2]] );
					selrx6Hist->fill( rx[indexconverter[3]] );
					selry6Hist->fill( ry[indexconverter[3]] );
					selrx3Hist->fill( rx[indexconverter[4]] );
					selry3Hist->fill( ry[indexconverter[4]] );
					selrx4Hist->fill( rx[indexconverter[5]] );
					selry4Hist->fill( ry[indexconverter[5]] );
					selrx5Hist->fill( rx[indexconverter[6]] );
					selry5Hist->fill( ry[indexconverter[6]] );

					if (_useREF == true)
					{
						selrx7Hist->fill( rx[indexconverter[7]] );
						selry7Hist->fill( ry[indexconverter[7]] );
					}

					double Chi2;
					int Ndf;
					double lostWeight;

					gbl::GblTrajectory traj(traj_points, _BField);
					traj.fit( Chi2, Ndf, lostWeight );
					traj.getLabels(ilab);

					streamlog_out (DEBUG3) << "Fitted track: chi2: " << Chi2 << ", ndf: " << Ndf << endl;

					gblndfHist->fill( Ndf );
					gblchi2Hist->fill( Chi2 );
					double chi2ndf = Chi2/(Ndf*1.0);
					gblchi2ndfHist->fill( chi2ndf );
					double probchi = TMath::Prob( Chi2, Ndf );
					gblprbHist->fill( probchi );

					// possibility to cut on track slope at DUT:
					TVectorD aCorrection(5);
					TMatrixDSym aCovariance(5);

					// bad fits:
					if( probchi < _probCut || chi2ndf > _chi2ndfCut || fabs(sxA[iA]*1E3) > _slopecutDUTx || fabs(syA[iA]*1E3) > _slopecutDUTy )
					{
						if (probchi < _probCut)
						{
							_probCutCount++;
							streamlog_out( DEBUG0 ) << "Track failed prob cut!" << endl;
						}
						if (chi2ndf > _chi2ndfCut)
						{
							_chi2ndfCutCount++;
							streamlog_out( DEBUG0 ) << "Track failed chi2ndf cut!" << endl;
						}
						if (fabs(sxA[iA]*1E3) > _slopecutDUTx)
						{
							_cutfailsx++;
							streamlog_out( DEBUG0 ) << "Track failed slope x cut!" << endl;
						}
						if (fabs(syA[iA]*1E3) > _slopecutDUTy)
						{
							_cutfailsy++;
							streamlog_out( DEBUG0 ) << "Track failed slope y cut!" << endl;
						}

						badxHist->fill( xA*1E-3 );	// triplet at DUT
						badyHist->fill( yA*1E-3 );
						badaxHist->fill( sxA[iA]*1E3 );	// track slope
						badayHist->fill( syA[iA]*1E3 );
						baddxHist->fill( dx );		// triplet-driplet match
						baddyHist->fill( dy );
						badkxHist->fill( kx*1E3 );	// triplet-driplet kink
						badkyHist->fill( ky*1E3 );

						baddx0Hist->fill( rx[indexconverter[0]] );
						baddy0Hist->fill( ry[indexconverter[0]] );
						baddx1Hist->fill( rx[indexconverter[1]] );
						baddy1Hist->fill( ry[indexconverter[1]] );
						baddx2Hist->fill( rx[indexconverter[2]] );
						baddy2Hist->fill( ry[indexconverter[2]] );
						baddx6Hist->fill( rx[indexconverter[3]] );
						baddy6Hist->fill( ry[indexconverter[3]] );
						baddx3Hist->fill( rx[indexconverter[4]] );
						baddy3Hist->fill( ry[indexconverter[4]] );
						baddx4Hist->fill( rx[indexconverter[5]] );
						baddy4Hist->fill( ry[indexconverter[5]] );
						baddx5Hist->fill( rx[indexconverter[6]] );
						baddy5Hist->fill( ry[indexconverter[6]] );

						if (_useREF == true)
						{
							baddx7Hist->fill( rx[indexconverter[7]] );
							baddy7Hist->fill( ry[indexconverter[7]] );
						}

					} else
					{
						streamlog_out( DEBUG0 ) << "Good track!" << endl;

						goodx0Hist->fill( rx[indexconverter[0]] );
						goody0Hist->fill( ry[indexconverter[0]] );
						goodx1Hist->fill( rx[indexconverter[1]] );
						goody1Hist->fill( ry[indexconverter[1]] );
						goodx2Hist->fill( rx[indexconverter[2]] );
						goody2Hist->fill( ry[indexconverter[2]] );
						goodx6Hist->fill( rx[indexconverter[3]] );
						goody6Hist->fill( ry[indexconverter[3]] );
						goodx3Hist->fill( rx[indexconverter[4]] );
						goody3Hist->fill( ry[indexconverter[4]] );
						goodx4Hist->fill( rx[indexconverter[5]] );
						goody4Hist->fill( ry[indexconverter[5]] );
						goodx5Hist->fill( rx[indexconverter[6]] );
						goody5Hist->fill( ry[indexconverter[6]] );

						if (_useREF == true)
						{
							goodx7Hist->fill( rx[indexconverter[7]] );
							goody7Hist->fill( ry[indexconverter[7]] );
						}

						// 6 telescope planes, + 1 dut + 1 ref
						double ax[8];
						double ay[8];
						unsigned int k = 0;
						unsigned int ndim = 2;

						TVectorD aResiduals(ndim);
						TVectorD aMeasErrors(ndim);
						TVectorD aResErrors(ndim);
						TVectorD aDownWeights(ndim);

						TVectorD aKinks(ndim);
						TVectorD aKinkErrors(ndim);
						TVectorD kResErrors(ndim);
						TVectorD kDownWeights(ndim);

						// prepare an encoder for the hit collection
						CellIDEncoder<TrackerHitImpl> fitHitEncoder(EUTELESCOPE::HITENCODING, fitpointvec);

						// Set flag for storing track hits in track collection

						LCFlagImpl flag(fittrackvec->getFlag());
						flag.setBit( LCIO::TRBIT_HITS );
						fittrackvec->setFlag(flag.getFlag());

						TrackImpl * fittrack = new TrackImpl();

						fittrack->setOmega(0.);		// curvature of the track
						fittrack->setD0(0.);		// impact paramter of the track in (r-phi)
						fittrack->setZ0(0.);		// impact paramter of the track in (r-z)
						fittrack->setPhi(0.);		// phi of the track at reference point
						fittrack->setTanLambda(0.);	// dip angle of the track at reference point

						fittrack->setChi2(Chi2);	// Chi2 of the fit (including penalties)
						fittrack->setNdf(Ndf);		// Number of planes

						double fitpos[3] = {0.0,0.0,0.0};
						float fitcov[TRKHITNCOVMATRIX] = {0.,0.,0.,0.,0.,0.};

						//track = q/p, x', y', x, y
						//        0,   1,  2,  3, 4

						// at plane 0:
						int ipos = 0;
						traj.getResults( ipos, aCorrection, aCovariance );
						// no results at 0
						// traj.getMeasResults( static_cast<unsigned int>(ipos), ndim, aResiduals, aMeasErrors, aResErrors, aDownWeights );
						// traj.getScatResults( static_cast<unsigned int>(ipos), ndim, aKinks, aKinkErrors, kResErrors, kDownWeights );
						gblax0Hist->fill( sxA[kA] - aCorrection[1]*1E3 );		// angle x [mrad]
						gbldx0Hist->fill( aCorrection[3] );				// shift x [um]
						gblrx0Hist->fill( rx[indexconverter[0]] - aCorrection[3] );	// residual x [um]
						gblay0Hist->fill( syA[kA] - aCorrection[2]*1E3 );		// angle y [mrad]
						gbldy0Hist->fill( aCorrection[4] );				// shift y [um]
						gblry0Hist->fill( ry[indexconverter[0]] - aCorrection[4] );	// residual y [um]
						gblpx0Hist->fill( 0.0 ); 					// pull residual x
						gblpy0Hist->fill( 0.0 ); 					// pull residual y 
						ax[k] = aCorrection[1];						// angle correction at plane, for kinks
						ay[k] = aCorrection[2];						// angle correction at plane, for kinks

						// the fit at this point is the measurement - aCorrection, so we can save these fit hits:
						TrackerHitImpl * fitpoint0 = new TrackerHitImpl;
						fitHitEncoder["sensorID"] =  0;
						fitHitEncoder["properties"] = kFittedHit;
						fitHitEncoder.setCellID( fitpoint0 );

						// for output go back to mm:
						jhit = hts[indexconverter[0]][kA];
						fitpos[0] = (_hitsArray[indexconverter[0]][jhit].measuredX - aCorrection[3])/1000.0;
						fitpos[1] = (_hitsArray[indexconverter[0]][jhit].measuredY - aCorrection[4])/1000.0;
						fitpos[2] = _hitsArray[indexconverter[0]][jhit].measuredZ/1000.0;

						fitpoint0->setPosition(fitpos);

						fitpoint0->setCovMatrix(fitcov);
						fitpointvec->push_back(fitpoint0);
						fittrack->addHit(fitpoint0);

						// for plane 0: this is the reference
						float refpoint[3];
						refpoint[0] = fitpos[0];
						refpoint[1] = fitpos[1];
						refpoint[2] = fitpos[2];

						k++;

						ipos = 1;
						traj.getResults( ipos, aCorrection, aCovariance );
						traj.getMeasResults( static_cast<unsigned int>(ipos), ndim, aResiduals, aMeasErrors, aResErrors, aDownWeights );
						traj.getScatResults( static_cast<unsigned int>(ipos), ndim, aKinks, aKinkErrors, kResErrors, kDownWeights );
						gblax1Hist->fill( sxA[kA] - aCorrection[1]*1E3 );
						gbldx1Hist->fill( aCorrection[3] );
						gblrx1Hist->fill( rx[indexconverter[1]] - aCorrection[3] );
						gblay1Hist->fill( syA[kA] - aCorrection[2]*1E3 );
						gbldy1Hist->fill( aCorrection[4] );
						gblry1Hist->fill( ry[indexconverter[1]] - aCorrection[4] );
						gblpx1Hist->fill( aResiduals[0] / aResErrors[0] );
						gblpy1Hist->fill( aResiduals[1] / aResErrors[1] ); 
						ax[k] = aCorrection[1];
						ay[k] = aCorrection[2];

						TrackerHitImpl * fitpoint1 = new TrackerHitImpl;
						fitHitEncoder["sensorID"] =  1;
						fitHitEncoder["properties"] = kFittedHit;
						fitHitEncoder.setCellID( fitpoint1 );

						// for output go back to mm:
						jhit = hts[indexconverter[1]][kA];
						fitpos[0] = (_hitsArray[indexconverter[1]][jhit].measuredX - aCorrection[3])/1000.0;
						fitpos[1] = (_hitsArray[indexconverter[1]][jhit].measuredY - aCorrection[4])/1000.0;
						fitpos[2] = _hitsArray[indexconverter[1]][jhit].measuredZ/1000.0;
	
						fitpoint1->setPosition(fitpos);
	
						fitpoint1->setCovMatrix(fitcov);
						fitpointvec->push_back(fitpoint1);
						fittrack->addHit(fitpoint1);
	
						k++;
	
						ipos = 2;
						traj.getResults( ipos, aCorrection, aCovariance );
						traj.getMeasResults( static_cast<unsigned int>(ipos), ndim, aResiduals, aMeasErrors, aResErrors, aDownWeights );
						traj.getScatResults( static_cast<unsigned int>(ipos), ndim, aKinks, aKinkErrors, kResErrors, kDownWeights );
						gblax2Hist->fill( sxA[kA] - aCorrection[1]*1E3 );
						gbldx2Hist->fill( aCorrection[3] );
						gblrx2Hist->fill( rx[indexconverter[2]] - aCorrection[3] );
						gblay2Hist->fill( syA[kA] - aCorrection[2]*1E3 );
						gbldy2Hist->fill( aCorrection[4] );
						gblry2Hist->fill( ry[indexconverter[2]] - aCorrection[4] );
						gblpx2Hist->fill( aResiduals[0] / aResErrors[0] );
						gblpy2Hist->fill( aResiduals[1] / aResErrors[1] );
						ax[k] = aCorrection[1];
						ay[k] = aCorrection[2];
	
						TrackerHitImpl * fitpoint2 = new TrackerHitImpl;
						fitHitEncoder["sensorID"] =  2;
						fitHitEncoder["properties"] = kFittedHit;
						fitHitEncoder.setCellID( fitpoint2 );
	
						// for output go back to mm:
						jhit = hts[indexconverter[2]][kA];
						fitpos[0] = (_hitsArray[indexconverter[2]][jhit].measuredX - aCorrection[3])/1000.0;
						fitpos[1] = (_hitsArray[indexconverter[2]][jhit].measuredY - aCorrection[4])/1000.0;
						fitpos[2] = _hitsArray[indexconverter[2]][jhit].measuredZ/1000.0;
	
						fitpoint2->setPosition(fitpos);
	
						fitpoint2->setCovMatrix(fitcov);
						fitpointvec->push_back(fitpoint2);
						fittrack->addHit(fitpoint2);
	
						k++;
	
						ipos = 3; // 6 = DUT
						traj.getResults( ipos, aCorrection, aCovariance );
						traj.getMeasResults( static_cast<unsigned int>(ipos), ndim, aResiduals, aMeasErrors, aResErrors, aDownWeights );
						traj.getScatResults( static_cast<unsigned int>(ipos), ndim, aKinks, aKinkErrors, kResErrors, kDownWeights );
						gblax6Hist->fill( sxA[kA] - aCorrection[1]*1E3 );
						gbldx6Hist->fill( aCorrection[3] );
						gblrx6Hist->fill( rx[indexconverter[3]] - aCorrection[3] );
						gblay6Hist->fill( syA[kA] - aCorrection[2]*1E3 );
						gbldy6Hist->fill( aCorrection[4] );
						gblry6Hist->fill( ry[indexconverter[3]] - aCorrection[4] );
						gblpx6Hist->fill( aResiduals[0] / aResErrors[0] );
						gblpy6Hist->fill( aResiduals[1] / aResErrors[1] );
						ax[k] = aCorrection[1];
						ay[k] = aCorrection[2];
	
						TrackerHitImpl * fitpoint3 = new TrackerHitImpl;
						fitHitEncoder["sensorID"] =  6;
						fitHitEncoder["properties"] = kFittedHit;
						fitHitEncoder.setCellID( fitpoint3 );
	
						// for output go back to mm:
						jhit = hts[indexconverter[3]][kA];
	
						// for the DUT, the point can come from a measurement or from the fit:
						if (_requireDUTHit == 0)
						{
							fitpos[0] = dutfitposX/1000.0;
							fitpos[1] = dutfitposY/1000.0;
							fitpos[2] = dutfitposZ/1000.0;
						} else
						{
							fitpos[0] = (_hitsArray[indexconverter[3]][jhit].measuredX - aCorrection[3])/1000.0;
							fitpos[1] = (_hitsArray[indexconverter[3]][jhit].measuredY - aCorrection[4])/1000.0;
							fitpos[2] = _hitsArray[indexconverter[3]][jhit].measuredZ/1000.0;
						}

						// fill detailed residuals
						dutrxxHist->fill( rx[indexconverter[3]] , fitpos[0]);
						dutrxyHist->fill( rx[indexconverter[3]] , fitpos[1]);
						dutrxzHist->fill( rx[indexconverter[3]] , (fitpos[2]-(_dutplane.Z()/1000.0)));
						dutrxxProf->Fill( fitpos[0] , rx[indexconverter[3]] , 1);
						dutrxyProf->Fill( fitpos[1] , rx[indexconverter[3]] , 1);
						dutrxzProf->Fill( (fitpos[2]-(_dutplane.Z()/1000.0)) , rx[indexconverter[3]] , 1);
						dutrxsxHist->fill( rx[indexconverter[3]] , sxA[iA]*1E3);
						dutrxsyHist->fill( rx[indexconverter[3]] , syA[iA]*1E3);
						dutrxkxHist->fill( rx[indexconverter[3]] , kx*1E3 );
						dutrxkyHist->fill( rx[indexconverter[3]] , ky*1E3 );
						dutrxchi2ndfHist->fill( rx[indexconverter[3]] , chi2ndf );
						dutrxprobHist->fill( rx[indexconverter[3]] , probchi );

						dutryxHist->fill( ry[indexconverter[3]] , fitpos[0]);
						dutryyHist->fill( ry[indexconverter[3]] , fitpos[1]);
						dutryzHist->fill( ry[indexconverter[3]] , (fitpos[2]-(_dutplane.Z()/1000.0)));
						dutryxProf->Fill( fitpos[0] , ry[indexconverter[3]] , 1);
						dutryyProf->Fill( fitpos[1] , ry[indexconverter[3]] ,1);
						dutryzProf->Fill( (fitpos[2]-(_dutplane.Z()/1000.0)) , ry[indexconverter[3]] , 1);
						dutrysxHist->fill( ry[indexconverter[3]] , sxA[iA]*1E3);
						dutrysyHist->fill( ry[indexconverter[3]] , syA[iA]*1E3);
						dutrykxHist->fill( ry[indexconverter[3]] , kx*1E3 );
						dutrykyHist->fill( ry[indexconverter[3]] , ky*1E3 );
						dutrychi2ndfHist->fill( ry[indexconverter[3]] , chi2ndf );
						dutryprobHist->fill( ry[indexconverter[3]] , probchi );

						// FIXME
						rz[indexconverter[3]] = dutfitposZ/1000.0 - fitpos[2];

						dutrzxHist->fill( rz[indexconverter[3]] , fitpos[0]);
						dutrzyHist->fill( rz[indexconverter[3]] , fitpos[1]);
						dutrzzHist->fill( rz[indexconverter[3]] , (fitpos[2]-(_dutplane.Z()/1000.0)));
						dutrzxProf->Fill( fitpos[0] , rz[indexconverter[3]] , 1);
						dutrzyProf->Fill( fitpos[1] , rz[indexconverter[3]] , 1);
						dutrzzProf->Fill( (fitpos[2]-(_dutplane.Z()/1000.0)) , rz[indexconverter[3]] , 1);
						dutrzsxHist->fill( rz[indexconverter[3]] , sxA[iA]*1E3);
						dutrzsyHist->fill( rz[indexconverter[3]] , syA[iA]*1E3);
						dutrzkxHist->fill( rz[indexconverter[3]] , kx*1E3 );
						dutrzkyHist->fill( rz[indexconverter[3]] , ky*1E3 );
						dutrzchi2ndfHist->fill( rz[indexconverter[3]] , chi2ndf );
						dutrzprobHist->fill( rz[indexconverter[3]] , probchi );

						// fill kink vs impact map
						dutkxmap->fill( fitpos[0], fitpos[1] , fabs(kx*1E3) );
						dutkymap->fill( fitpos[0], fitpos[1] , fabs(ky*1E3) );
						dutkmap->fill( fitpos[0], fitpos[1] , (kx*1E3*kx*1E3 + ky*1E3*ky*1E3) );

						// hitmap for comparison
						duthitmap->fill( fitpos[0], fitpos[1] );
						duttrackhitmap->fill( dutfitposX/1000.0 , dutfitposY/1000.0 );

						fitpoint3->setPosition(fitpos);
	
						fitpoint3->setCovMatrix(fitcov);
						fitpointvec->push_back(fitpoint3);
						fittrack->addHit(fitpoint3);
	
						k++;
	
						ipos = 4;
						traj.getResults( ipos, aCorrection, aCovariance );
						traj.getMeasResults( static_cast<unsigned int>(ipos), ndim, aResiduals, aMeasErrors, aResErrors, aDownWeights );
						traj.getScatResults( static_cast<unsigned int>(ipos), ndim, aKinks, aKinkErrors, kResErrors, kDownWeights );
						gblax3Hist->fill( sxB[kB] - aCorrection[1]*1E3 );
						gbldx3Hist->fill( aCorrection[3] );
						gblrx3Hist->fill( rx[indexconverter[4]] - aCorrection[3] );
						gblay3Hist->fill( syB[kB] - aCorrection[2]*1E3 );
						gbldy3Hist->fill( aCorrection[4] );
						gblry3Hist->fill( ry[indexconverter[4]] - aCorrection[4] );
						gblpx3Hist->fill( aResiduals[0] / aResErrors[0] );
						gblpy3Hist->fill( aResiduals[1] / aResErrors[1] );
						ax[k] = aCorrection[1];
						ay[k] = aCorrection[2];
	
						TrackerHitImpl * fitpoint4 = new TrackerHitImpl;
						fitHitEncoder["sensorID"] =  3;
						fitHitEncoder["properties"] = kFittedHit;
						fitHitEncoder.setCellID( fitpoint4 );
	
						// for output go back to mm:
						jhit = hts[indexconverter[4]][kB];
						fitpos[0] = (_hitsArray[indexconverter[4]][jhit].measuredX - aCorrection[3])/1000.0;
						fitpos[1] = (_hitsArray[indexconverter[4]][jhit].measuredY - aCorrection[4])/1000.0;
						fitpos[2] = _hitsArray[indexconverter[4]][jhit].measuredZ/1000.0;
	
						fitpoint4->setPosition(fitpos);
	
						fitpoint4->setCovMatrix(fitcov);
						fitpointvec->push_back(fitpoint4);
						fittrack->addHit(fitpoint4);
	
						k++;
	
						ipos = 5;
						traj.getResults( ipos, aCorrection, aCovariance );
						traj.getMeasResults( static_cast<unsigned int>(ipos), ndim, aResiduals, aMeasErrors, aResErrors, aDownWeights );
						traj.getScatResults( static_cast<unsigned int>(ipos), ndim, aKinks, aKinkErrors, kResErrors, kDownWeights );
						gblax4Hist->fill( sxB[kB] - aCorrection[1]*1E3 );
						gbldx4Hist->fill( aCorrection[3] );
						gblrx4Hist->fill( rx[indexconverter[5]] - aCorrection[3] );
						gblay4Hist->fill( syB[kB] - aCorrection[2]*1E3 );
						gbldy4Hist->fill( aCorrection[4] );
						gblry4Hist->fill( ry[indexconverter[5]] - aCorrection[4] );
						gblpx4Hist->fill( aResiduals[0] / aResErrors[0] );
						gblpy4Hist->fill( aResiduals[1] / aResErrors[1] );
						ax[k] = aCorrection[1];
						ay[k] = aCorrection[2];
	
						TrackerHitImpl * fitpoint5 = new TrackerHitImpl;
						fitHitEncoder["sensorID"] =  4;
						fitHitEncoder["properties"] = kFittedHit;
						fitHitEncoder.setCellID( fitpoint5 );
	
						// for output go back to mm:
						jhit = hts[indexconverter[5]][kB];
						fitpos[0] = (_hitsArray[indexconverter[5]][jhit].measuredX - aCorrection[3])/1000.0;
						fitpos[1] = (_hitsArray[indexconverter[5]][jhit].measuredY - aCorrection[4])/1000.0;
						fitpos[2] = _hitsArray[indexconverter[5]][jhit].measuredZ/1000.0;
	
						fitpoint5->setPosition(fitpos);
	
						fitpoint5->setCovMatrix(fitcov);
						fitpointvec->push_back(fitpoint5);
						fittrack->addHit(fitpoint5);
	
						k++;
	
						ipos = 6;
						traj.getResults( ipos, aCorrection, aCovariance );
						traj.getMeasResults( static_cast<unsigned int>(ipos), ndim, aResiduals, aMeasErrors, aResErrors, aDownWeights );
						traj.getScatResults( static_cast<unsigned int>(ipos), ndim, aKinks, aKinkErrors, kResErrors, kDownWeights );
						gblax5Hist->fill( sxB[kB] - aCorrection[1]*1E3 );
						gbldx5Hist->fill( aCorrection[3] );
						gblrx5Hist->fill( rx[indexconverter[6]] - aCorrection[3] );
						gblay5Hist->fill( syB[kB] - aCorrection[2]*1E3 );
						gbldy5Hist->fill( aCorrection[4] );
						gblry5Hist->fill( ry[indexconverter[6]] - aCorrection[4] );
						gblpx5Hist->fill( aResiduals[0] / aResErrors[0] );
						gblpy5Hist->fill( aResiduals[1] / aResErrors[1] );
						ax[k] = aCorrection[1];
						ay[k] = aCorrection[2];
	
						TrackerHitImpl * fitpoint6 = new TrackerHitImpl;
						fitHitEncoder["sensorID"] =  5;
						fitHitEncoder["properties"] = kFittedHit;
						fitHitEncoder.setCellID( fitpoint6 );
	
						// for output go back to mm:
						jhit = hts[indexconverter[6]][kB];
						fitpos[0] = (_hitsArray[indexconverter[6]][jhit].measuredX - aCorrection[3])/1000.0;
						fitpos[1] = (_hitsArray[indexconverter[6]][jhit].measuredY - aCorrection[4])/1000.0;
						fitpos[2] = _hitsArray[indexconverter[6]][jhit].measuredZ/1000.0;
	
						fitpoint6->setPosition(fitpos);
	
						fitpoint6->setCovMatrix(fitcov);
						fitpointvec->push_back(fitpoint6);
						fittrack->addHit(fitpoint6);
	
						k++;

						if (_useREF == true)
						{
							ipos = 7;
							traj.getResults( ipos, aCorrection, aCovariance );
							traj.getMeasResults( static_cast<unsigned int>(ipos), ndim, aResiduals, aMeasErrors, aResErrors, aDownWeights );
							traj.getScatResults( static_cast<unsigned int>(ipos), ndim, aKinks, aKinkErrors, kResErrors, kDownWeights );
							gblax7Hist->fill( sxB[kB] - aCorrection[1]*1E3 );
							gbldx7Hist->fill( aCorrection[3] );
							gblrx7Hist->fill( rx[indexconverter[7]] - aCorrection[3] );
							gblay7Hist->fill( syB[kB] - aCorrection[2]*1E3 );
							gbldy7Hist->fill( aCorrection[4] );
							gblry7Hist->fill( ry[indexconverter[7]] - aCorrection[4] );
							gblpx7Hist->fill( aResiduals[0] / aResErrors[0] );
							gblpy7Hist->fill( aResiduals[1] / aResErrors[1] );
							ax[k] = aCorrection[1];
							ay[k] = aCorrection[2];
		
							TrackerHitImpl * fitpoint7 = new TrackerHitImpl;
							fitHitEncoder["sensorID"] =  7;
							fitHitEncoder["properties"] = kFittedHit;
							fitHitEncoder.setCellID( fitpoint7 );
		
							// for output go back to mm:
							jhit = hts[indexconverter[7]][kB];
							fitpos[0] = (_hitsArray[indexconverter[7]][jhit].measuredX - aCorrection[3])/1000.0;
							fitpos[1] = (_hitsArray[indexconverter[7]][jhit].measuredY - aCorrection[4])/1000.0;
							fitpos[2] = _hitsArray[indexconverter[7]][jhit].measuredZ/1000.0;
		
							fitpoint7->setPosition(fitpos);
		
							fitpoint7->setCovMatrix(fitcov);
							fitpointvec->push_back(fitpoint7);
							fittrack->addHit(fitpoint7);
		
							k++;
						}
	
						// done with the track
						fittrack->setReferencePoint(refpoint);
						fittrackvec->addElement(fittrack);
	
	
						gblkx1Hist->fill( (ax[indexconverter[1]] - ax[indexconverter[0]])*1E3 ); // kink at 1 [mrad]
						gblkx2Hist->fill( (ax[indexconverter[2]] - ax[indexconverter[1]])*1E3 ); // kink at 2 [mrad]
						gblkx6Hist->fill( (ax[indexconverter[3]] - ax[indexconverter[2]])*1E3 ); // kink at DUT [mrad]
						gblkx3Hist->fill( (ax[indexconverter[4]] - ax[indexconverter[3]])*1E3 ); // kink at 3 [mrad]
						gblkx4Hist->fill( (ax[indexconverter[5]] - ax[indexconverter[4]])*1E3 ); // kink at 4 [mrad]
						gblkx5Hist->fill( (ax[indexconverter[6]] - ax[indexconverter[5]])*1E3 ); // kink at 5 [mrad]

						if (_useREF == true)
						{
							gblkx7Hist->fill( (ax[indexconverter[7]] - ax[indexconverter[7]])*1E3 ); // kink at REF [mrad]
						}
	
						gblky1Hist->fill( (ay[indexconverter[1]] - ay[indexconverter[0]])*1E3 ); // kink at 1 [mrad]
						gblky2Hist->fill( (ay[indexconverter[2]] - ay[indexconverter[1]])*1E3 ); // kink at 2 [mrad]
						gblky6Hist->fill( (ay[indexconverter[3]] - ay[indexconverter[2]])*1E3 ); // kink at DUT [mrad]
						gblky3Hist->fill( (ay[indexconverter[4]] - ay[indexconverter[3]])*1E3 ); // kink at 3 [mrad]
						gblky4Hist->fill( (ay[indexconverter[5]] - ay[indexconverter[4]])*1E3 ); // kink at 4 [mrad]
						gblky5Hist->fill( (ay[indexconverter[6]] - ay[indexconverter[5]])*1E3 ); // kink at 5 [mrad]

						if (_useREF == true)
						{
							gblky7Hist->fill( (ay[indexconverter[7]] - ay[indexconverter[7]])*1E3 ); // kink at REF [mrad]
						}

						// done
						streamlog_message( DEBUG0, traj.printTrajectory(10);, std::endl; );

						// output to millepede
						nmGood++;
						traj.milleOut( *milleGBL );
						_nGoodMilleTracks++;

					} // prob and chi2ndf cut

				} // match

			}//loop kB

		}//loop kA
		

		nmGoodHist->fill( nmGood );
		nmHist->fill( nm );

		_nMilleTracks += nm;

	} // done no plane excluded

	// count events:
	++_iEvt;
	if( isFirstEvent() )
	{
		_isFirstEvent = false;
	}

	// add track to lcio
	if (_doPreAlignment == 0)
	{
		event->addCollection(fittrackvec,_outputTrackCollectionName);
		event->addCollection(fitpointvec,_outputHitCollectionName);
	}

	// fill the rate histos
	if (event->getEventNumber() > 0 && _dthistos == true && _doPreAlignment == 0)
	{
		trackrate->fill(event->getEventNumber(),(_nMilleTracks*1.0/(event->getEventNumber()*1.0)));
		badtrackrate->fill(event->getEventNumber(),((_nMilleTracks - _nGoodMilleTracks)*1.0/(event->getEventNumber()*1.0)));
		goodtrackrate->fill(event->getEventNumber(),(_nGoodMilleTracks*1.0/(event->getEventNumber()*1.0)));
	}

} // done


void EUTelMilleGBL::end()
{

	// close the output file:
	delete milleGBL;

	// if write the pede steering file
	if( _generatePedeSteerfile && _doPreAlignment == 0)
	{

		streamlog_out( MESSAGE4 ) << endl << "Generating the steering file for the pede program..." << endl;

		ofstream steerFile;
		steerFile.open(_pedeSteerfileName.c_str());

		if( steerFile.is_open())
		{

			// find first and last excluded plane
			int firstnotexcl = _nPlanes;
			int lastnotexcl = 0;

			// loop over all planes:

			for( int ipl = 0; ipl < _nPlanes; ipl++)
			{

				int excluded = 0;

				// loop over excluded planes:

				for( int jpl = 0; jpl < _nExcludePlanes; jpl++ )
				{
					if( ipl == _excludePlanes[jpl] )
					{
						excluded = 1;
					}
				}

				if( excluded == 0 && firstnotexcl > ipl )
				{
					firstnotexcl = ipl;
				}

				if( excluded == 0 && lastnotexcl < ipl )
				{
					lastnotexcl = ipl;
				}

			} // end loop over all planes

			steerFile << "Cfiles" << endl;
			steerFile << _binaryFilename << endl;
			steerFile << endl;

			steerFile << "Parameter" << endl;

			int counter = 0;
			int nfix = 0;

			// loop over all planes:
			for( int ipl = 0; ipl < _nPlanes; ipl++)
			{
				// flag for excluded planes
				int excluded = 0;

				// check in list of excluded planes:
				for( int iex = 0; iex < _nExcludePlanes; iex++)
				{
					if( ipl == _excludePlanes[iex] )
					{
						excluded = 1;
					}
				}

				streamlog_out (MESSAGE4)<< "Plane " << ipl << " exclude = " << excluded << endl;

				if( excluded == 0 )
				{

					bool fixed = false;
					for( size_t i = 0;i< _FixedPlanes.size(); i++ )
					{

						if( _FixedPlanes[i] == ipl )
						{
							fixed = true;
						}
					}

					// if fixed planes
					if( fixed || (_FixedPlanes.size() == 0 && (ipl == firstnotexcl || ipl == lastnotexcl) ) )
					{
						nfix++;
						if( _alignMode == 1 )
						{
							steerFile << (counter * 6 + 1) << "  0.0 -1.0" << endl; // fix x
							steerFile << (counter * 6 + 2) << "  0.0 -1.0" << endl; // fix y
							steerFile << (counter * 6 + 3) << "  0.0 -1.0" << endl; // fix z
							steerFile << (counter * 6 + 4) << "  0.0 -1.0" << endl; // fix a
							steerFile << (counter * 6 + 5) << "  0.0 -1.0" << endl; // fix b
							steerFile << (counter * 6 + 6) << "  0.0 -1.0" << endl; // fix g
						}
						if( _alignMode == 2 )
						{
							steerFile << (counter * 2 + 1) << "  0.0 -1.0" << endl;
							steerFile << (counter * 2 + 2) << "  0.0 -1.0" << endl;
						}
						if( _alignMode == 3 )
						{
							steerFile << (counter * 3 + 1) << "  0.0 -1.0" << endl; // fix x
							steerFile << (counter * 3 + 2) << "  0.0 -1.0" << endl; // fix y
							steerFile << (counter * 3 + 3) << "  0.0 -1.0" << endl; // fix rot
						}
						if( _alignMode == 4 )
						{
							steerFile << (counter * 4 + 1) << "  0.0 -1.0" << endl;
							steerFile << (counter * 4 + 2) << "  0.0 -1.0" << endl;
							steerFile << (counter * 4 + 3) << "  0.0 -1.0" << endl;
						}
						if( _alignMode == 5 || _alignMode == 6)
						{
							steerFile << (counter * 6 + 1) << "  0.0 -1.0" << endl; // fix x
							steerFile << (counter * 6 + 2) << "  0.0 -1.0" << endl; // fix y
							steerFile << (counter * 6 + 3) << "  0.0 -1.0" << endl; // fix z
							steerFile << (counter * 6 + 4) << "  0.0 -1.0" << endl; // fix a
							steerFile << (counter * 6 + 5) << "  0.0 -1.0" << endl; // fix b
							steerFile << (counter * 6 + 6) << "  0.0 -1.0" << endl; // fix g
						}
						if( _alignMode == 7 )
						{
							steerFile << (counter * 3 + 1) << "  0.0 -1.0" << endl; // fix x
							steerFile << (counter * 3 + 2) << "  0.0 -1.0" << endl; // fix y
							steerFile << (counter * 3 + 3) << "  0.0 -1.0" << endl; // fix z
						}

					} else
					{

						if( _alignMode == 1 )
						{
							steerFile << (counter * 6 + 1) << "  0.0 -1.0" << endl; // fix x
							steerFile << (counter * 6 + 2) << "  0.0 -1.0" << endl; // fix y
							
							if(_FixParameter[ipl] & (1 << 2))
							{
								steerFile << (counter * 6 + 3) << "  0.0 -1.0" << endl; // fix z
							} else {
							    	steerFile << (counter * 6 + 3) << "  0.0  0.0" << endl; // free z
							}
							steerFile << (counter * 6 + 4) << "  0.0 -1.0" << endl; // fix a
							steerFile << (counter * 6 + 5) << "  0.0 -1.0" << endl; // fix b
							steerFile << (counter * 6 + 6) << "  0.0 -1.0" << endl; // fix g
						}
						if( _alignMode == 2 )
						{
							if(_FixParameter[ipl] & (1 << 0))
							{
								steerFile << (counter * 2 + 1) << "  0.0 -1.0" << endl;
							} else {
								steerFile << (counter * 2 + 1) << "  0.0  0.0" << endl;
							}
							if(_FixParameter[ipl] & (1 << 1))
							{
								steerFile << (counter * 2 + 2) << "  0.0 -1.0" << endl;
							} else {
								steerFile << (counter * 2 + 2) << "  0.0  0.0" << endl;
							}
						}
						if( _alignMode == 3 )
						{
							if(_FixParameter[ipl] & (1 << 0))
							{
								steerFile << (counter * 3 + 1) << "  0.0 -1.0" << endl;
							} else {
								steerFile << (counter * 3 + 1) << "  0.0  0.0" << endl;
							}
							if(_FixParameter[ipl] & (1 << 1))
							{
								steerFile << (counter * 3 + 2) << "  0.0 -1.0" << endl;
							} else {
								steerFile << (counter * 3 + 2) << "  0.0  0.0" << endl;
							}
							if(_FixParameter[ipl] & (1 << 2))
							{
								steerFile << (counter * 3 + 3) << "  0.0 -1.0" << endl;
							} else {
								steerFile << (counter * 3 + 3) << "  0.0  0.0" << endl;
							}
						}
						if( _alignMode == 4 )
						{
							if(_FixParameter[ipl] & (1 << 0))
							{
								steerFile << (counter * 4 + 1) << "  0.0 -1.0" << endl;
							} else {
								steerFile << (counter * 4 + 1) << "  0.0  0.0" << endl;
							}
							if(_FixParameter[ipl] & (1 << 1))
							{
								steerFile << (counter * 4 + 2) << "  0.0 -1.0" << endl;
							} else {
								steerFile << (counter * 4 + 2) << "  0.0  0.0" << endl;
							}
							if(_FixParameter[ipl] & (1 << 2))
							{
								steerFile << (counter * 4 + 3) << "  0.0 -1.0" << endl;
							} else {
								steerFile << (counter * 4 + 3) << "  0.0  0.0" << endl;
							}
						}
						if( _alignMode == 5 )
						{
							if(_FixParameter[ipl] & (1 << 0))
							{
								steerFile << (counter * 6 + 1) << "  0.0 -1.0" << endl; // fix x
							} else {
								steerFile << (counter * 6 + 1) << "  0.0  0.0" << endl; // free x
							}
							if(_FixParameter[ipl] & (1 << 1))
							{
								steerFile << (counter * 6 + 2) << "  0.0 -1.0" << endl; // fix y
							} else {
								steerFile << (counter * 6 + 2) << "  0.0  0.0" << endl; // free y
							}
							steerFile << (counter * 6 + 3) << "  0.0 -1.0" << endl; // fix z
							if(_FixParameter[ipl] & (1 << 3))
							{
								steerFile << (counter * 6 + 4) << "  0.0 -1.0" << endl; // fix a
							} else {
								steerFile << (counter * 6 + 4) << "  0.0  0.0" << endl; // free a
							}
							if(_FixParameter[ipl] & (1 << 4))
							{
								steerFile << (counter * 6 + 5) << "  0.0 -1.0" << endl; // fix b
							} else {
								steerFile << (counter * 6 + 5) << "  0.0  0.0" << endl; // free b
							}
							if(_FixParameter[ipl] & (1 << 5))
							{
								steerFile << (counter * 6 + 6) << "  0.0 -1.0" << endl; // fix g
							} else {
								steerFile << (counter * 6 + 6) << "  0.0  0.0" << endl; // free g
							}
						}
						if( _alignMode == 6 )
						{
							if(_FixParameter[ipl] & (1 << 0))
							{
								steerFile << (counter * 6 + 1) << "  0.0 -1.0" << endl; // fix x
							} else {
								steerFile << (counter * 6 + 1) << "  0.0  0.0" << endl; // free x
							}
							if(_FixParameter[ipl] & (1 << 1))
							{
								steerFile << (counter * 6 + 2) << "  0.0 -1.0" << endl; // fix y
							} else {
								steerFile << (counter * 6 + 2) << "  0.0  0.0" << endl; // free y
							}
							if(_FixParameter[ipl] & (1 << 2))
							{
								steerFile << (counter * 6 + 3) << "  0.0 -1.0" << endl; // fix z
							} else {
								steerFile << (counter * 6 + 3) << "  0.0  0.0" << endl; // free z
							}
							if(_FixParameter[ipl] & (1 << 3))
							{
								steerFile << (counter * 6 + 4) << "  0.0 -1.0" << endl; // fix a
							} else {
								steerFile << (counter * 6 + 4) << "  0.0  0.0" << endl; // free a
							}
							if(_FixParameter[ipl] & (1 << 4))
							{
								steerFile << (counter * 6 + 5) << "  0.0 -1.0" << endl; // fix b
							} else {
								steerFile << (counter * 6 + 5) << "  0.0  0.0" << endl; // free b
							}
							if(_FixParameter[ipl] & (1 << 5))
							{
								steerFile << (counter * 6 + 6) << "  0.0 -1.0" << endl; // fix g
							} else {
								steerFile << (counter * 6 + 6) << "  0.0  0.0" << endl; // free g
							}
						}
						if( _alignMode == 7 )
						{
							if(_FixParameter[ipl] & (1 << 0))
							{
								steerFile << (counter * 3 + 1) << "  0.0 -1.0" << endl;
							} else {
								steerFile << (counter * 3 + 1) << "  0.0  0.0" << endl;
							}
							if(_FixParameter[ipl] & (1 << 1))
							{
								steerFile << (counter * 3 + 2) << "  0.0  -1.0" << endl;
							} else {
								steerFile << (counter * 3 + 2) << "  0.0  0.0" << endl;
							}
							if(_FixParameter[ipl] & (1 << 2))
							{
								steerFile << (counter * 3 + 3) << "  0.0 -1.0" << endl;
							} else {
								steerFile << (counter * 3 + 3) << "  0.0  0.0" << endl;
							}
						}

					} // not fixed

					/* FIXME
					// special for z shift:
					if( _alignMode == 4 || _alignMode == 1)
					{
						if( ipl == 1 )
						{
							steerFile << (counter * 4 + 4) << "  0.0 -1.0" << endl;
						} else if( ipl == 4 )
						{
							steerFile << (counter * 4 + 4) << "  0.0 -1.0" << endl;
						} else
						{
							steerFile << (counter * 4 + 4) << "  0.0  0.0" << endl;
						}
					}
					*/

					counter++;

				} // end if plane not excluded

			} // end loop over all planes

		if( nfix < 2 )
			{

				if( _alignMode == 2 )
				{

					steerFile << "Constraint 0 ! sum dx = 0" << endl;
					steerFile << " 1  1.0" << endl;
					steerFile << " 3  1.0" << endl;
					steerFile << " 5  1.0" << endl;
					steerFile << " 7  1.0" << endl;
					steerFile << " 9  1.0" << endl;
					steerFile << "11  1.0" << endl;
			
					steerFile << "Constraint 0 ! sum dy = 0" << endl;
					steerFile << " 2  1.0" << endl;
					steerFile << " 4  1.0" << endl;
					steerFile << " 6  1.0" << endl;
					steerFile << " 8  1.0" << endl;
					steerFile << "10  1.0" << endl;
					steerFile << "12  1.0" << endl;
				}

				if( _alignMode == 3 )
				{

					steerFile << "Constraint 0 ! sum dx = 0" << endl;
					steerFile << " 1  1.0" << endl;
					steerFile << " 4  1.0" << endl;
					steerFile << " 7  1.0" << endl;
					steerFile << "10  1.0" << endl;
					steerFile << "13  1.0" << endl;
					steerFile << "16  1.0" << endl;
					steerFile << "19  1.0" << endl;
			
					steerFile << "Constraint 0 ! sum dy = 0" << endl;
					steerFile << " 2  1.0" << endl;
					steerFile << " 5  1.0" << endl;
					steerFile << " 8  1.0" << endl;
					steerFile << "11  1.0" << endl;
					steerFile << "14  1.0" << endl;
					steerFile << "17  1.0" << endl;
					steerFile << "20  1.0" << endl;
			
					steerFile << "Constraint 0 ! sum dphi = 0" << endl;
					steerFile << " 3  1.0" << endl;
					steerFile << " 6  1.0" << endl;
					steerFile << " 9  1.0" << endl;
					steerFile << "12  1.0" << endl;
					steerFile << "15  1.0" << endl;
					steerFile << "18  1.0" << endl;
					steerFile << "21  1.0" << endl;
				}

				if( _alignMode == 4 )
				{

					steerFile << "Constraint 0 ! sum dx = 0" << endl;
					steerFile << " 1  1.0" << endl;
					steerFile << " 5  1.0" << endl;
					steerFile << " 9  1.0" << endl;
					steerFile << "13  1.0" << endl;
					steerFile << "17  1.0" << endl;
					steerFile << "21  1.0" << endl;
			
					steerFile << "Constraint 0 ! sum dy = 0" << endl;
					steerFile << " 2  1.0" << endl;
					steerFile << " 6  1.0" << endl;
					steerFile << "10  1.0" << endl;
					steerFile << "14  1.0" << endl;
					steerFile << "18  1.0" << endl;
					steerFile << "22  1.0" << endl;
			
					steerFile << "Constraint 0 ! sum dphi = 0" << endl;
					steerFile << " 3  1.0" << endl;
					steerFile << " 7  1.0" << endl;
					steerFile << "11  1.0" << endl;
					steerFile << "15  1.0" << endl;
					steerFile << "19  1.0" << endl;
					steerFile << "23  1.0" << endl;
			
					steerFile << "Constraint 0 ! sum dz = 0" << endl;
					steerFile << " 4  1.0" << endl;
					steerFile << " 8  1.0" << endl;
					steerFile << "12  1.0" << endl;
					steerFile << "16  1.0" << endl;
					steerFile << "20  1.0" << endl;
					steerFile << "24  1.0" << endl;
				}

				if( _alignMode == 5 || _alignMode == 6 || _alignMode == 1)
				{

					steerFile << "Constraint 0 ! sum dx = 0" << endl;
					steerFile << " 1  1.0" << endl;
					steerFile << " 7  1.0" << endl;
					steerFile << "13  1.0" << endl;
					steerFile << "19  1.0" << endl;
					steerFile << "25  1.0" << endl;
					steerFile << "31  1.0" << endl;
					steerFile << "37  1.0" << endl;

					steerFile << "Constraint 0 ! sum dy = 0" << endl;
					steerFile << " 2  1.0" << endl;
					steerFile << " 8  1.0" << endl;
					steerFile << "14  1.0" << endl;
					steerFile << "20  1.0" << endl;
					steerFile << "26  1.0" << endl;
					steerFile << "32  1.0" << endl;
					steerFile << "38  1.0" << endl;

					steerFile << "Constraint 0 ! sum dz = 0" << endl;
					steerFile << " 3  1.0" << endl;
					steerFile << " 9  1.0" << endl;
					steerFile << "15  1.0" << endl;
					steerFile << "21  1.0" << endl;
					steerFile << "27  1.0" << endl;
					steerFile << "33  1.0" << endl;
					steerFile << "39  1.0" << endl;

					steerFile << "Constraint 0 ! sum dtheta = 0" << endl;
					steerFile << " 4  1.0" << endl;
					steerFile << "10  1.0" << endl;
					steerFile << "16  1.0" << endl;
					steerFile << "22  1.0" << endl;
					steerFile << "28  1.0" << endl;
					steerFile << "34  1.0" << endl;
					steerFile << "40  1.0" << endl;

					steerFile << "Constraint 0 ! sum dpsi = 0" << endl;
					steerFile << " 5  1.0" << endl;
					steerFile << "11  1.0" << endl;
					steerFile << "17  1.0" << endl;
					steerFile << "23  1.0" << endl;
					steerFile << "29  1.0" << endl;
					steerFile << "35  1.0" << endl;
					steerFile << "41  1.0" << endl;

					steerFile << "Constraint 0 ! sum dphi = 0" << endl;
					steerFile << " 6  1.0" << endl;
					steerFile << "12  1.0" << endl;
					steerFile << "18  1.0" << endl;
					steerFile << "24  1.0" << endl;
					steerFile << "30  1.0" << endl;
					steerFile << "36  1.0" << endl;
					steerFile << "42  1.0" << endl;

				}

			} //nfix < 2

			steerFile << endl;
			steerFile << "! chiscut 5.0 2.5" << endl;
			steerFile << "outlierdownweighting 4" << endl;
			steerFile << "dwfractioncut 0.1" << endl;
			steerFile << endl;
			steerFile << "method inversion 10  0.1" << endl;
			// Use 10 OpenMP threads to process the data:
			steerFile << "threads 10 1" << endl;
			steerFile << endl;
			steerFile << "histprint" << endl;
			steerFile << endl;
			steerFile << "end" << endl;

			steerFile.close();

			streamlog_out( MESSAGE4 ) << "File " << _pedeSteerfileName << " written." << endl;

		} else // steerfile open
		{
			streamlog_out( ERROR2 ) << "Could not open steering file." << endl;
		}

	} // end if write the pede steering file


	if (_doPreAlignment == 0)
	{

		streamlog_out( MESSAGE4 ) << endl;
		streamlog_out( MESSAGE4 ) << endl;
		streamlog_out( MESSAGE4 ) << "====================================================" <<endl;
		streamlog_out( MESSAGE4 ) << "Number of tracks:                         " << _nMilleTracks << endl;
		streamlog_out( MESSAGE4 ) << "Number of good tracks used:               " << _nGoodMilleTracks << endl;
		streamlog_out( MESSAGE4 ) << "====================================================" <<endl;
		streamlog_out( MESSAGE4 ) << "Number of tracks failing chi2ndf cut:     " << _chi2ndfCutCount << endl;
		streamlog_out( MESSAGE4 ) << "Number of tracks failing prob cut:        " << _probCutCount << endl;
		streamlog_out( MESSAGE4 ) << "Number of tracks failing DUT slope x cut: " << _cutfailsx << endl;
		streamlog_out( MESSAGE4 ) << "Number of tracks failing DUT slope y cut: " << _cutfailsy << endl;
		streamlog_out( MESSAGE4 ) << endl;
		streamlog_out( MESSAGE4 ) << endl;
	}

	// if running pede using the generated steering file
	if( _runPede == 1 && _nGoodMilleTracks > 0 && _doPreAlignment == 0 && _doDUTAlignment == 0)
	{

		// check if steering file exists
		if( _generatePedeSteerfile == 1 )
		{

			std::string command = "pede " + _pedeSteerfileName;

			// before starting pede, let's check if it is in the path
			bool isPedeInPath = true;

			// create a new process
			redi::ipstream which("which pede");

			// wait for the process to finish
			which.close();

			// get the status
			// if it 255 then the program wasn't found in the path
			isPedeInPath = !( which.rdbuf()->status() == 255 );

			if( !isPedeInPath )
			{
				streamlog_out( ERROR5 ) << "Pede cannot be executed because not found in the path" << endl;
			} else
			{

				streamlog_out( MESSAGE2 ) << endl;
				streamlog_out( MESSAGE2 ) << "Starting pede..." << endl;
				streamlog_out( MESSAGE2 ) << command.c_str() << endl;

				redi::ipstream pede( command.c_str() );
				string output;
				while ( getline( pede, output ) )
				{
					streamlog_out( MESSAGE2 ) << output << endl;
				}

				// wait for the pede execution to finish
				pede.close();

				// check the exit value of pede
				if( pede.rdbuf()->status() == 0 )
				{
					streamlog_out( MESSAGE4 ) << "Pede successfully finished" << endl;
				}

				// reading back the millepede.res file:

				string millepedeResFileName = "millepede.res";

				streamlog_out( MESSAGE4 ) << "Reading back the " << millepedeResFileName << endl;
				streamlog_out( MESSAGE4 ) << "Saving the alignment constant into " << _alignmentConstantLCIOFile << endl;

				// open the millepede ASCII output file
				ifstream millepede( millepedeResFileName.c_str() );

				// reopen the LCIO file this time in append mode
				LCWriter * lcWriter = LCFactory::getInstance()->createLCWriter();

				try {
					lcWriter->open( _alignmentConstantLCIOFile, LCIO::WRITE_NEW );
				}
				catch ( IOException& e )
				{
					streamlog_out( ERROR5 ) << e.what() << endl;
					streamlog_out( ERROR5 ) << "Sorry for quitting. " << endl;
					exit(-1);
				}

				// write an almost empty run header
				LCRunHeaderImpl * lcHeader  = new LCRunHeaderImpl;
				lcHeader->setRunNumber( 0 );

				lcWriter->writeRunHeader(lcHeader);

				delete lcHeader;

				LCEventImpl * event = new LCEventImpl;
				event->setRunNumber( 0 );
				event->setEventNumber( 0 );

				LCTime * now = new LCTime;
				event->setTimeStamp( now->timeStamp() );
				delete now;

				LCCollectionVec * constantsCollection = new LCCollectionVec( LCIO::LCGENERICOBJECT );

				if( millepede.bad() || !millepede.is_open() )
				{
					streamlog_out( ERROR5 ) << "Error opening the " << millepedeResFileName << endl;
					streamlog_out( ERROR5 ) << "The alignment slcio file cannot be saved" << endl;
				} else
				{
					vector<double > tokens;
					stringstream tokenizer;
					string line;

					// get the first line and throw it away since it is a comment!

					getline( millepede, line );
					streamlog_out (DEBUG3) << "line:" <<  line  << endl;

					int counter = 0;

					while( ! millepede.eof() )
					{

						EUTelAlignmentConstant * constant = new EUTelAlignmentConstant;

						bool goodLine = true;

						unsigned int numpars = 2;
						if(_alignMode == 3 || _alignMode == 7)
						{
							numpars = 3;
						} else if (_alignMode == 4)
						{
							numpars=4;
						} else if (_alignMode == 5 || _alignMode == 6 || _alignMode == 1)
						{
							numpars = 6;
						}

						for ( unsigned int iParam = 0 ; iParam < numpars ; ++iParam ) 
						{
							getline( millepede, line );

							if ( line.empty() )
							{
								goodLine = false;
								continue;
							}

							tokens.clear();
							tokenizer.clear();
							tokenizer.str( line );

							double buffer;
							// // check that all parts of the line are non zero
							while ( tokenizer >> buffer )
							{
								tokens.push_back( buffer ) ;
							}

							if ( ( tokens.size() == 3 ) || ( tokens.size() == 6 ) || (tokens.size() == 5) )
							{
								goodLine = true;
							} else {
								goodLine = false;
							}

							bool isFixed = ( tokens.size() == 3 );

							// only x, y and gamma
							if(_alignMode != 5 && _alignMode != 6 && _alignMode != 1 && _alignMode != 7)
							{
								if ( iParam == 0 )
								{
									constant->setXOffset( tokens[1] / 1000. );
									if ( ! isFixed )
									{
										constant->setXOffsetError( tokens[4] / 1000. ) ;
									}
								}
								if ( iParam == 1 )
								{
									constant->setYOffset( tokens[1] / 1000. ) ;
									if ( ! isFixed )
									{
										constant->setYOffsetError( tokens[4] / 1000. ) ;
									}
								}
								if ( iParam == 2 )
								{
									constant->setGamma( tokens[1]  ) ;
									if ( ! isFixed )
									{
										constant->setGammaError( tokens[4] ) ;
									}
								}
							} else if(_alignMode == 7)
							{
								if ( iParam == 0 )
								{
									constant->setXOffset( tokens[1] / 1000. );
									if ( ! isFixed )
									{
										constant->setXOffsetError( tokens[4] / 1000. ) ;
									}
								}
								if ( iParam == 1 )
								{
									constant->setYOffset( tokens[1] / 1000. ) ;
									if ( ! isFixed )
									{
										constant->setYOffsetError( tokens[4] / 1000. ) ;
									}
								}
								if ( iParam == 2 )
								{
									constant->setZOffset( tokens[1]  ) ;
									if ( ! isFixed )
									{
										constant->setZOffsetError( tokens[4] ) ;
									}
								}
							} else
							// all constants
							{
								if ( iParam == 0 )
								{
									constant->setXOffset( tokens[1] / 1000. );
									if ( ! isFixed )
									{
										constant->setXOffsetError( tokens[4] / 1000. ) ;
									}
								}
								if ( iParam == 1 )
								{
									constant->setYOffset( tokens[1] / 1000. ) ;
									if ( ! isFixed )
									{
										constant->setYOffsetError( tokens[4] / 1000. ) ;
									}
								}
								if ( iParam == 2 )
								{
									constant->setZOffset( tokens[1] / 1000. ) ;
									if ( ! isFixed )
									{
										constant->setZOffsetError( tokens[4] / 1000. ) ;
									}
								}
								if ( iParam == 3 )
								{
									constant->setAlpha( tokens[1]  ) ;
									if ( ! isFixed )
									{
										constant->setAlphaError( tokens[4] ) ;
									}
								} 
								if ( iParam == 4 )
								{
									constant->setBeta( tokens[1]  ) ;
									if ( ! isFixed )
									{
										constant->setBetaError( tokens[4] ) ;
									}
								} 
								if ( iParam == 5 )
								{
									constant->setGamma( tokens[1]  ) ;
									if ( ! isFixed )
									{
										constant->setGammaError( tokens[4] ) ;
									}
								}

							}

						} // done par loop

						// right place to add the constant to the collection
						if ( goodLine  )
						{
							// constant->setSensorID( _orderedSensorID_wo_excluded.at( counter ) );
							constant->setSensorID( _orderedSensorID.at( counter ) );
							++ counter;
							constantsCollection->push_back( constant );
							streamlog_out ( MESSAGE0 ) << (*constant) << endl;

						}	else
						{
							delete constant;
						}

					} //millepede eof

					// catch the case that there is a reference plane that is not used, write out an empty set of alignment constants
					if (counter < _nPlanes)
					{
						EUTelAlignmentConstant * constant = new EUTelAlignmentConstant;
						streamlog_out (MESSAGE2) << "Writing empty constants for reference plane!" << endl;
						constant->setSensorID(8); //HACK
						constant->setXOffset(0.0);
						constant->setYOffset(0.0);
						constant->setZOffset(0.0);
						constant->setAlpha(0.0);
						constant->setBeta(0.0);
						constant->setGamma(0.0);
						constantsCollection->push_back( constant );

					}

				} //millepede OK

				event->addCollection( constantsCollection, _alignmentConstantCollectionName );
				lcWriter->writeEvent( event );
				delete event;

				lcWriter->close();

				millepede.close();

			} //PedeInPath

		// Pede steering exist
		} else
		{
			streamlog_out( ERROR2 ) << "Unable to run pede. No steering file has been generated." << endl;
		}

	} else if (_runPede == 1 && _nGoodMilleTracks == 0 && _doPreAlignment == 0 && _doDUTAlignment == 0)
	{
		streamlog_out( WARNING2 ) << " " << endl;
		streamlog_out( WARNING2 ) << "No good tracks found! Writing empty alignment constant file!" << endl;
		streamlog_out( WARNING2 ) << " " << endl;
		
		// writer
		LCWriter * lcWriter = LCFactory::getInstance()->createLCWriter();
		try 
		{
			lcWriter->open( _alignmentConstantLCIOFile, LCIO::WRITE_NEW );
		} catch ( IOException& e )
		{
			streamlog_out ( ERROR5 ) << e.what() << endl;
			exit(-1);
		}

		// write an almost empty run header
		LCRunHeaderImpl * lcHeader  = new LCRunHeaderImpl;
		lcHeader->setRunNumber( 0 );
		lcWriter->writeRunHeader(lcHeader);
		delete lcHeader;

		// an event:
		LCEventImpl * event = new LCEventImpl;
		event->setRunNumber( 0 );
		event->setEventNumber( 0 );

		// the alignment constant collection we want to write
		LCCollectionVec * constantsCollection = new LCCollectionVec( LCIO::LCGENERICOBJECT );

		for ( int i = 0; i<_nPlanes;i++)
		{
			EUTelAlignmentConstant * constant = new EUTelAlignmentConstant;
			constant->setSensorID(i);
			constant->setXOffset(0.0);
			constant->setYOffset(0.0);
			constant->setZOffset(0.0);
			constant->setAlpha(0.0);
			constant->setBeta(0.0);
			constant->setGamma(0.0);
			constantsCollection->push_back( constant );
		}

		event->addCollection( constantsCollection, _alignmentConstantCollectionName );

		// output all this
		lcWriter->writeEvent( event );
		delete event;
		lcWriter->close();

		streamlog_out ( WARNING2 ) << "Done writing empty file!" << endl;

	}
	if (_doDUTAlignment != 0)
	{
		streamlog_out( MESSAGE2 ) << "Doing DUT rotation alignment" << endl;
		double additionalrot[3] = {0.0};

		// FIXME for now only y info...
		if (_doDUTAlignment == 1)
		{
			streamlog_out( MESSAGE2 ) << "Angle is alpha!" << endl;
			// find the limits of the plot, so that we don't include the edges of the sensor...
			double lowbin = 0;
			double hibin = 150;
			for (int i=0;i<150;i++)
			{
				if (fabs(dutryyProf->GetBinContent(i))>0 && dutryyProf->GetBinEntries(i) >= 7)
				{
					hibin = i;
				}
			}
			for (int i=149;i>=0;i--)
			{
				if (fabs(dutryyProf->GetBinContent(i))>0 && fabs(dutryyProf->GetBinContent(i))<100.0 && dutryyProf->GetBinEntries(i) >= 7)
				{
					lowbin = i;
				}
			}
			// -15.0 for the histo range, / 5.0 for the bin per um
			double x_min = -15.0 + lowbin / 5.0;
			double x_max = -15.0 + hibin / 5.0;
			double range = x_max - x_min;
			x_min += (range/5.0);
			x_max -= (range/5.0);
			profilefitalpha->SetRange(x_min,x_max);
			streamlog_out ( DEBUG5 ) << "Lower bin: " << lowbin << " , upper bin: " << hibin << " !" << endl;
			streamlog_out ( DEBUG5 ) << "Lower pos: " << x_min << " , upper pos: " << x_max << " !" << endl;
			dutryyProf->Fit(profilefitalpha,"QR");

			// get an additional XY rotation for alignment, /1000 for um->mm:
			additionalrot[0] = atan((profilefitalpha->GetParameter(1))/1000.0);
			streamlog_out ( MESSAGE2 ) << "Alpha rotation is: " << additionalrot[0] << " rad!" << endl;
		}
		if (_doDUTAlignment == 3)
		{
			streamlog_out( MESSAGE2 ) << "Angle is gamma!" << endl;
			// find the limits of the plot, so that we don't include the edges of the sensor...
			double lowbin = 0;
			double hibin = 150;
			for (int i=0;i<150;i++)
			{
				if (fabs(dutryxProf->GetBinContent(i))>0 && dutryxProf->GetBinEntries(i) >= 7)
				{
					hibin = i;
				}
			}
			for (int i=149;i>=0;i--)
			{
				if (fabs(dutryxProf->GetBinContent(i))>0 && fabs(dutryxProf->GetBinContent(i))<100.0 && dutryxProf->GetBinEntries(i) >= 7)
				{
					lowbin = i;
				}
			}
			// -15.0 for the histo range, / 5.0 for the bin per um
			double x_min = -15.0 + lowbin / 5.0;
			double x_max = -15.0 + hibin / 5.0;
			double range = x_max - x_min;
			x_min += (range/5.0);
			x_max -= (range/5.0);
			profilefitgamma->SetRange(x_min,x_max);
			streamlog_out ( DEBUG5 ) << "Lower bin: " << lowbin << " , upper bin: " << hibin << " !" << endl;
			streamlog_out ( DEBUG5 ) << "Lower pos: " << x_min << " , upper pos: " << x_max << " !" << endl;
			dutryxProf->Fit(profilefitgamma,"QR");

			// get an additional XY rotation for alignment, /1000 for um->mm:
			additionalrot[2] = atan((profilefitgamma->GetParameter(1))/1000.0);
			streamlog_out ( MESSAGE2 ) << "Gamma rotation is: " << additionalrot[2] << " rad!" << endl;
		}

		// writer
		LCWriter * lcWriter = LCFactory::getInstance()->createLCWriter();
		try 
		{
			lcWriter->open( _alignmentConstantLCIOFile, LCIO::WRITE_NEW );
		} catch ( IOException& e )
		{
			streamlog_out ( ERROR5 ) << e.what() << endl;
			exit(-1);
		}

		// write an almost empty run header
		LCRunHeaderImpl * lcHeader  = new LCRunHeaderImpl;
		lcHeader->setRunNumber( 0 );
		lcWriter->writeRunHeader(lcHeader);
		delete lcHeader;

		// an event:
		LCEventImpl * event = new LCEventImpl;
		event->setRunNumber( 0 );
		event->setEventNumber( 0 );

		// the alignment constant collection we want to write
		LCCollectionVec * constantsCollection = new LCCollectionVec( LCIO::LCGENERICOBJECT );

		for ( int i = 0; i<_nPlanes;i++ )
		{
			if (_orderedSensorID.at( i ) != _manualDUTid )
			{
				EUTelAlignmentConstant * constant = new EUTelAlignmentConstant;
				constant->setSensorID( _orderedSensorID.at( i ) );
				constant->setXOffset(0.0);
				constant->setYOffset(0.0);
				constant->setZOffset(0.0);
				constant->setAlpha(0.0);
				constant->setBeta(0.0);
				constant->setGamma(0.0);
				constantsCollection->push_back( constant );
			} else {
				EUTelAlignmentConstant * constant = new EUTelAlignmentConstant;
				constant->setSensorID( _orderedSensorID.at( i ) );
				constant->setXOffset(0.0);
				constant->setYOffset(0.0);
				constant->setZOffset(0.0);
				constant->setAlpha(additionalrot[0]);
				constant->setBeta(additionalrot[1]);
				constant->setGamma(additionalrot[2]);
				constantsCollection->push_back( constant );
			}
		}

		event->addCollection( constantsCollection, _alignmentConstantCollectionName );

		// output all this
		lcWriter->writeEvent( event );
		delete event;
		lcWriter->close();

	}
	if (_doPreAlignment != 0 && _doDUTAlignment == 0)
	{

		streamlog_out( MESSAGE2 ) << "Doing prealignment..." << endl;
		
		double prealignx[8] = {0.0};
		double prealigny[8] = {0.0};
		int nb = 0;
		int nmax = 0;
		double temp = 0.0;

		nb = dx01Hist->axis().bins();
		for (int j=0;j<nb;j++)
		{
			if (dx01Hist->binHeight(j)> nmax)
			{
				nmax = static_cast<int>(dx01Hist->binHeight(j));
				temp = dx01Hist->binMean(j)/1000.0;
			}
		}
		prealignx[1] = temp;
		streamlog_out( MESSAGE2 ) << "Plane 1 x: " << temp;

		nb = dy01Hist->axis().bins();
		nmax = 0;
		for (int j=0;j<nb;j++)
		{
			if (dy01Hist->binHeight(j)> nmax)
			{
				nmax = static_cast<int>(dy01Hist->binHeight(j));
				temp = dy01Hist->binMean(j)/1000.0;
			}
		}
		prealigny[1] = temp;
		streamlog_out( MESSAGE2 ) << " mm, y: " << temp << " mm." << endl;

		nb = dx02Hist->axis().bins();
		nmax = 0;
		for (int j=0;j<nb;j++)
		{
			if (dx02Hist->binHeight(j)> nmax)
			{
				nmax = static_cast<int>(dx02Hist->binHeight(j));
				temp = dx02Hist->binMean(j)/1000.0;
			}
		}
		prealignx[2] = temp;
		streamlog_out( MESSAGE2 ) << "Plane 2 x: " << temp;

		nb = dy02Hist->axis().bins();
		nmax = 0;
		for (int j=0;j<nb;j++)
		{
			if (dy02Hist->binHeight(j)> nmax)
			{
				nmax = static_cast<int>(dy02Hist->binHeight(j));
				temp = dy02Hist->binMean(j)/1000.0;
			}
		}
		prealigny[2] = temp;
		streamlog_out( MESSAGE2 ) << " mm, y: " << temp << " mm." << endl;

		// DUT 
		nb = dx06Hist->axis().bins();
		nmax = 0;
		for (int j=0;j<nb;j++)
		{
			if (dx06Hist->binHeight(j)> nmax)
			{
				nmax = static_cast<int>(dx06Hist->binHeight(j));
				temp = dx06Hist->binMean(j)/1000.0;
			}
		}
		prealignx[3] = temp;
		streamlog_out( MESSAGE2 ) << "DUT x: " << temp;

		nb = dy06Hist->axis().bins();
		nmax = 0;
		for (int j=0;j<nb;j++)
		{
			if (dy06Hist->binHeight(j)> nmax)
			{
				nmax = static_cast<int>(dy06Hist->binHeight(j));
				temp = dy06Hist->binMean(j)/1000.0;
			}
		}
		prealigny[3] = temp;
		streamlog_out( MESSAGE2 ) << " mm, y: " << temp << " mm." << endl;

		nb = dx03Hist->axis().bins();
		nmax = 0;
		for (int j=0;j<nb;j++)
		{
			if (dx03Hist->binHeight(j)> nmax)
			{
				nmax = static_cast<int>(dx03Hist->binHeight(j));
				temp = dx03Hist->binMean(j)/1000.0;
			}
		}
		prealignx[4] = temp;
		streamlog_out( MESSAGE2 ) << "Plane 3 x: " << temp;

		nb = dy03Hist->axis().bins();
		nmax = 0;
		for (int j=0;j<nb;j++)
		{
			if (dy03Hist->binHeight(j)> nmax)
			{
				nmax = static_cast<int>(dy03Hist->binHeight(j));
				temp = dy03Hist->binMean(j)/1000.0;
			}
		}
		prealigny[4] = temp;
		streamlog_out( MESSAGE2 ) << " mm, y: " << temp << " mm." << endl;

		nb = dx04Hist->axis().bins();
		nmax = 0;
		for (int j=0;j<nb;j++)
		{
			if (dx04Hist->binHeight(j)> nmax)
			{
				nmax = static_cast<int>(dx04Hist->binHeight(j));
				temp = dx04Hist->binMean(j)/1000.0;
			}
		}
		prealignx[5] = temp;
		streamlog_out( MESSAGE2 ) << "Plane 4 x: " << temp;

		nb = dy04Hist->axis().bins();
		nmax = 0;
		for (int j=0;j<nb;j++)
		{
			if (dy04Hist->binHeight(j)> nmax)
			{
				nmax = static_cast<int>(dy04Hist->binHeight(j));
				temp = dy04Hist->binMean(j)/1000.0;
			}
		}
		prealigny[5] = temp;
		streamlog_out( MESSAGE2 ) << " mm, y: " << temp << " mm." << endl;

		nb = dx05Hist->axis().bins();
		nmax = 0;
		for (int j=0;j<nb;j++)
		{
			if (dx05Hist->binHeight(j)> nmax)
			{
				nmax = static_cast<int>(dx05Hist->binHeight(j));
				temp = dx05Hist->binMean(j)/1000.0;
			}
		}
		prealignx[6] = temp;
		streamlog_out( MESSAGE2 ) << "Plane 5 x: " << temp;

		nb = dy05Hist->axis().bins();
		nmax = 0;
		for (int j=0;j<nb;j++)
		{
			if (dy05Hist->binHeight(j)> nmax)
			{
				nmax = static_cast<int>(dy05Hist->binHeight(j));
				temp = dy05Hist->binMean(j)/1000.0;
			}
		}
		prealigny[6] = temp;
		streamlog_out( MESSAGE2 ) << " mm, y: " << temp << " mm." << endl;

		if (_useREF == true)
		{
			nb = dx07Hist->axis().bins();
			nmax = 0;
			for (int j=0;j<nb;j++)
			{
				if (dx07Hist->binHeight(j)> nmax)
				{
					nmax = static_cast<int>(dx07Hist->binHeight(j));
					temp = dx07Hist->binMean(j)/1000.0;
				}
			}
			prealignx[7] = temp;
			streamlog_out( MESSAGE2 ) << "REF x: " << temp;

			nb = dy07Hist->axis().bins();
			nmax = 0;
			for (int j=0;j<nb;j++)
			{
				if (dy07Hist->binHeight(j)> nmax)
				{
					nmax = static_cast<int>(dy07Hist->binHeight(j));
					temp = dy07Hist->binMean(j)/1000.0;
				}
			}
			prealigny[7] = temp;
			streamlog_out( MESSAGE2 ) << " mm, y: " << temp << " mm." << endl;
		}

		if (_doPreAlignment == 2)
		{
			streamlog_out( MESSAGE2 ) << "Zero prealignment selected! Setting all prealignment values to 0!" << endl;
			for (int j = 0;j<_nPlanes;j++)
			{
				prealignx[j] = 0.0;
				prealigny[j] = 0.0;
			}
		}

		// writer
		LCWriter * lcWriter = LCFactory::getInstance()->createLCWriter();
		try 
		{
			lcWriter->open( _alignmentConstantLCIOFile, LCIO::WRITE_NEW );
		} catch ( IOException& e )
		{
			streamlog_out ( ERROR5 ) << e.what() << endl;
			exit(-1);
		}

		// write an almost empty run header
		LCRunHeaderImpl * lcHeader  = new LCRunHeaderImpl;
		lcHeader->setRunNumber( 0 );
		lcWriter->writeRunHeader(lcHeader);
		delete lcHeader;

		// an event:
		LCEventImpl * event = new LCEventImpl;
		event->setRunNumber( 0 );
		event->setEventNumber( 0 );

		// the alignment constant collection we want to write
		LCCollectionVec * constantsCollection = new LCCollectionVec( LCIO::LCGENERICOBJECT );

		for ( int i = 0; i<_nPlanes;i++ )
		{
			EUTelAlignmentConstant * constant = new EUTelAlignmentConstant;
			constant->setSensorID( _orderedSensorID.at( i ) );
			constant->setXOffset(-1.0 * prealignx[i]);
			constant->setYOffset(-1.0 * prealigny[i]);
			constant->setZOffset(0.0);
			constant->setAlpha(0.0);
			constant->setBeta(0.0);
			constant->setGamma(0.0);
			constantsCollection->push_back( constant );
		}

		event->addCollection( constantsCollection, _alignmentConstantCollectionName );

		// output all this
		lcWriter->writeEvent( event );
		delete event;
		lcWriter->close();

	}
	
	streamlog_out( MESSAGE2 ) << endl;
	streamlog_out( MESSAGE2 ) << "Successfully finished" << endl;

	// Remove mille binary
	if( remove( _binaryFilename.c_str() ) != 0 )
	{
		streamlog_out( ERROR5 ) << "Failed to delete mille binary file!" << endl;
	} else {
		streamlog_out( MESSAGE2 ) << "Cleaning up: successfully removed mille binary file." << endl;
	}

} //end


void EUTelMilleGBL::bookHistos()
{

	#if defined(USE_AIDA) || defined(MARLIN_USE_AIDA)

	try {
		streamlog_out( MESSAGE4 ) << "Booking histograms..." << endl;
		
		AIDAProcessor::tree(this)->mkdir("InputHits");

		dx01Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx01", 100, -5000, 5000 );
		dx01Hist->setTitle( "Hit Shift Plane 1 - Plane 0 in x;x_{1}-x_{0} [um];hit pairs" );

		dy01Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy01", 100, -5000, 5000 );
		dy01Hist->setTitle( "Hit Shift Plane 1 - Plane 0 in y;y_{1}-y_{0} [um];hit pairs" );

		dx02Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx02", 100, -5000, 5000 );
		dx02Hist->setTitle( "Hit Shift Plane 2 - Plane 0 in x;x_{2}-x_{0} [um];hit pairs" );

		dy02Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy02", 100, -5000, 5000 );
		dy02Hist->setTitle( "Hit Shift Plane 2 - Plane 0 in y;y_{2}-y_{0} [um];hit pairs" );

		dx03Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx03", 100, -5000, 5000 );
		dx03Hist->setTitle( "Hit Shift Plane 3 - Plane 0 in x;x_{3}-x_{0} [um];hit pairs" );

		dy03Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy03", 100, -5000, 5000 );
		dy03Hist->setTitle( "Hit Shift Plane 3 - Plane 0 in y;y_{3}-y_{0} [um];hit pairs" );

		dx04Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx04", 100, -5000, 5000 );
		dx04Hist->setTitle( "Hit Shift Plane 4 - Plane 0 in x;x_{4}-x_{0} [um];hit pairs" );

		dy04Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy04", 100, -5000, 5000 );
		dy04Hist->setTitle( "Hit Shift Plane 4 - Plane 0 in y;y_{4}-y_{0} [um];hit pairs" );

		dx05Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx05", 100, -5000, 5000 );
		dx05Hist->setTitle( "Hit Shift Plane 5 - Plane 0 in x;x_{5}-x_{0} [um];hit pairs" );

		dy05Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy05", 100, -5000, 5000 );
		dy05Hist->setTitle( "Hit Shift Plane 5 - Plane 0 in y;y_{5}-y_{0} [um];hit pairs" );

		dx06Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx06", 100, -5000, 5000 );
		dx06Hist->setTitle( "Hit Shift DUT - Plane 0 in x;x_{6}-x_{0} [um];hit pairs" );

		dy06Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy06", 100, -5000, 5000 );
		dy06Hist->setTitle( "Hit Shift DUT - Plane 0 in y;y_{6}-y_{0} [um];hit pairs" );


		if (_dthistos == true && _doPreAlignment == 0)
		{
			AIDAProcessor::tree(this)->mkdir("InputHits/Time");

			dx01dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dx01dt", 10000, 0, 500000, 100, -5000, 5000 );
			dx01dtHist->setTitle( "Hit Shift Plane 1 - Plane 0 in x;Event Number;x_{1}-x_{0} [um]" );

			dy01dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dy01dt", 10000, 0, 500000, 100, -5000, 5000 );
			dy01dtHist->setTitle( "Hit Shift Plane 1 - Plane 0 in y;Event Number;y_{1}-y_{0} [um]" );

			dx12dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dx12dt", 10000, 0, 500000, 100, -5000, 5000 );
			dx12dtHist->setTitle( "Hit Shift Plane 2 - Plane 1 in x;Event Number;x_{2}-x_{1} [um]" );

			dy12dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dy12dt", 10000, 0, 500000, 100, -5000, 5000 );
			dy12dtHist->setTitle( "Hit Shift Plane 2 - Plane 1 in y;Event Number;y_{2}-y_{1} [um]" );

			dx23dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dx23dt", 10000, 0, 500000, 100, -5000, 5000 );
			dx23dtHist->setTitle( "Hit Shift Plane 3 - Plane 2 in x;Event Number;x_{3}-x_{2} [um]" );

			dy23dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dy23dt", 10000, 0, 500000, 100, -5000, 5000 );
			dy23dtHist->setTitle( "Hit Shift Plane 3 - Plane 2 in y;Event Number;y_{3}-y_{2} [um]" );

			dx34dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dx34dt", 10000, 0, 500000, 100, -5000, 5000 );
			dx34dtHist->setTitle( "Hit Shift Plane 4 - Plane 3 in x;Event Number;x_{4}-x_{3} [um]" );

			dy34dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dy34dt", 10000, 0, 500000, 100, -5000, 5000 );
			dy34dtHist->setTitle( "Hit Shift Plane 4 - Plane 3 in y;Event Number;y_{4}-y_{3} [um]" );

			dx45dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dx45dt", 10000, 0, 500000, 100, -5000, 5000 );
			dx45dtHist->setTitle( "Hit Shift Plane 5 - Plane 4 in x;Event Number;x_{5}-x_{4} [um]" );

			dy45dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dy45dt", 10000, 0, 500000, 100, -5000, 5000 );
			dy45dtHist->setTitle( "Hit Shift Plane 5 - Plane 4 in y;Event Number;y_{5}-y_{4} [um]" );

			dx26dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dx26dt", 10000, 0, 500000, 100, -5000, 5000 );
			dx26dtHist->setTitle( "Hit Shift DUT - Plane 2 in x;Event Number;x_{6}-x_{2} [um]" );

			dy26dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dy26dt", 10000, 0, 500000, 100, -5000, 5000 );
			dy26dtHist->setTitle( "Hit Shift DUT - Plane 2 in y;Event Number;y_{6}-y_{2} [um]" );
		}

		if (_useREF == true)
		{

			dx07Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx07", 100, -5000, 5000 );
			dx07Hist->setTitle( "Hit Shift REF - Plane 0 in x;x_{7}-x_{0} [um];hit pairs" );

			dy07Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy07", 100, -5000, 5000 );
			dy07Hist->setTitle( "Hit Shift REF - Plane 0 in y;y_{7}-y_{0} [um];hit pairs" );

			if (_dthistos == true && _doPreAlignment == 0)
			{
				dx57dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dx57dt", 10000, 0, 500000, 100, -5000, 5000 );
				dx57dtHist->setTitle( "Hit Shift REF - Plane 5 in x;Event Number;x_{7}-x_{5} [um]" );

				dy57dtHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "InputHits/Time/dy57dt", 10000, 0, 500000, 100, -5000, 5000 );
				dy57dtHist->setTitle( "Hit Shift REF - Plane 5 in y;Event Number;y_{7}-y_{5} [um]" );
			}
		}

		if (_doPreAlignment == 0)
		{

			dx12Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx12", 100, -5000, 5000 );
			dx12Hist->setTitle( "Hit Shift Plane 2 - Plane 1 in x;x_{2}-x_{1} [um];hit pairs" );

			dy12Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy12", 100, -5000, 5000 );
			dy12Hist->setTitle( "Hit Shift Plane 2 - Plane 1 in y;y_{2}-y_{1} [um];hit pairs" );

			dx13Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx13", 100, -5000, 5000 );
			dx13Hist->setTitle( "Hit Shift Plane 3 - Plane 1 in x;x_{3}-x_{1} [um];hit pairs" );

			dy13Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy13", 100, -5000, 5000 );
			dy13Hist->setTitle( "Hit Shift Plane 3 - Plane 1 in y;y_{3}-y_{1} [um];hit pairs" );

			dx14Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx14", 100, -5000, 5000 );
			dx14Hist->setTitle( "Hit Shift Plane 4 - Plane 1 in x;x_{4}-x_{1} [um];hit pairs" );

			dy14Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy14", 100, -5000, 5000 );
			dy14Hist->setTitle( "Hit Shift Plane 4 - Plane 1 in y;y_{4}-y_{1} [um];hit pairs" );

			dx15Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx15", 100, -5000, 5000 );
			dx15Hist->setTitle( "Hit Shift Plane 5 - Plane 1 in x;x_{5}-x_{1} [um];hit pairs" );

			dy15Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy15", 100, -5000, 5000 );
			dy15Hist->setTitle( "Hit Shift Plane 5 - Plane 1 in y;y_{5}-y_{1} [um];hit pairs" );

			dx16Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx16", 100, -5000, 5000 );
			dx16Hist->setTitle( "Hit Shift DUT - Plane 1 in x;x_{6}-x_{1} [um];hit pairs" );

			dy16Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy16", 100, -5000, 5000 );
			dy16Hist->setTitle( "Hit Shift DUT - Plane 1 in y;y_{6}-y_{1} [um];hit pairs" );

			if (_useREF == true)
			{

				dx17Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx17", 100, -5000, 5000 );
				dx17Hist->setTitle( "Hit Shift REF - Plane 1 in x;x_{7}-x_{1} [um];hit pairs" );

				dy17Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy17", 100, -5000, 5000 );
				dy17Hist->setTitle( "Hit Shift REF - Plane 1 in y;y_{7}-y_{1} [um];hit pairs" );
			}


			dx23Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx23", 100, -5000, 5000 );
			dx23Hist->setTitle( "Hit Shift Plane 3 - Plane 2 in x;x_{3}-x_{2} [um];hit pairs" );

			dy23Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy23", 100, -5000, 5000 );
			dy23Hist->setTitle( "Hit Shift Plane 3 - Plane 2 in y;y_{3}-y_{2} [um];hit pairs" );

			dx24Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx24", 100, -5000, 5000 );
			dx24Hist->setTitle( "Hit Shift Plane 4 - Plane 2 in x;x_{4}-x_{2} [um];hit pairs" );

			dy24Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy24", 100, -5000, 5000 );
			dy24Hist->setTitle( "Hit Shift Plane 4 - Plane 2 in y;y_{4}-y_{2} [um];hit pairs" );

			dx25Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx25", 100, -5000, 5000 );
			dx25Hist->setTitle( "Hit Shift Plane 5 - Plane 2 in x;x_{5}-x_{2} [um];hit pairs" );

			dy25Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy25", 100, -5000, 5000 );
			dy25Hist->setTitle( "Hit Shift Plane 5 - Plane 2 in y;y_{5}-y_{2} [um];hit pairs" );

			dx26Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx26", 100, -5000, 5000 );
			dx26Hist->setTitle( "Hit Shift DUT - Plane 2 in x;x_{6}-x_{2} [um];hit pairs" );

			dy26Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy26", 100, -5000, 5000 );
			dy26Hist->setTitle( "Hit Shift DUT - Plane 2 in y;y_{6}-y_{2} [um];hit pairs" );

			if (_useREF == true)
			{

				dx27Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx27", 100, -5000, 5000 );
				dx27Hist->setTitle( "Hit Shift REF - Plane 1 in x;x_{7}-x_{2} [um];hit pairs" );

				dy27Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy27", 100, -5000, 5000 );
				dy27Hist->setTitle( "Hit Shift REF - Plane 1 in y;y_{7}-y_{2} [um];hit pairs" );
			}


			dx34Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx34", 100, -5000, 5000 );
			dx34Hist->setTitle( "Hit Shift Plane 4 - Plane 3 in x;x_{4}-x_{3} [um];hit pairs" );

			dy34Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy34", 100, -5000, 5000 );
			dy34Hist->setTitle( "Hit Shift Plane 4 - Plane 3 in y;y_{4}-y_{3} [um];hit pairs" );

			dx35Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx35", 100, -5000, 5000 );
			dx35Hist->setTitle( "Hit Shift Plane 5 - Plane 3 in x;x_{5}-x_{3} [um];hit pairs" );

			dy35Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy35", 100, -5000, 5000 );
			dy35Hist->setTitle( "Hit Shift Plane 5 - Plane 3 in y;y_{5}-y_{3} [um];hit pairs" );

			dx36Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx36", 100, -5000, 5000 );
			dx36Hist->setTitle( "Hit Shift DUT - Plane 3 in x;x_{6}-x_{3} [um];hit pairs" );

			dy36Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy36", 100, -5000, 5000 );
			dy36Hist->setTitle( "Hit Shift DUT - Plane 3 in y;y_{6}-y_{3} [um];hit pairs" );

			if (_useREF == true)
			{

				dx37Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx37", 100, -5000, 5000 );
				dx37Hist->setTitle( "Hit Shift REF - Plane 3 in x;x_{7}-x_{3} [um];hit pairs" );

				dy37Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy37", 100, -5000, 5000 );
				dy37Hist->setTitle( "Hit Shift REF - Plane 3 in y;y_{7}-y_{3} [um];hit pairs" );
			}


			dx45Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx45", 100, -5000, 5000 );
			dx45Hist->setTitle( "Hit Shift Plane 5 - Plane 4 in x;x_{5}-x_{4} [um];hit pairs" );

			dy45Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy45", 100, -5000, 5000 );
			dy45Hist->setTitle( "Hit Shift Plane 5 - Plane 4 in y;y_{5}-y_{4} [um];hit pairs" );

			dx46Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx46", 100, -5000, 5000 );
			dx46Hist->setTitle( "Hit Shift DUT - Plane 4 in x;x_{6}-x_{4} [um];hit pairs" );

			dy46Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy46", 100, -5000, 5000 );
			dy46Hist->setTitle( "Hit Shift DUT - Plane 4 in y;y_{6}-y_{4} [um];hit pairs" );

			if (_useREF == true)
			{

				dx47Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx47", 100, -5000, 5000 );
				dx47Hist->setTitle( "Hit Shift REF - Plane 4 in x;x_{7}-x_{4} [um];hit pairs" );

				dy47Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy47", 100, -5000, 5000 );
				dy47Hist->setTitle( "Hit Shift REF - Plane 4 in y;y_{7}-y_{4} [um];hit pairs" );
			}


			dx56Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx56", 100, -5000, 5000 );
			dx56Hist->setTitle( "Hit Shift DUT - Plane 5 in x;x_{6}-x_{5} [um];hit pairs" );

			dy56Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy56", 100, -5000, 5000 );
			dy56Hist->setTitle( "Hit Shift DUT - Plane 5 in y;y_{6}-y_{5} [um];hit pairs" );

			if (_useREF == true)
			{

				dx57Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx57", 100, -5000, 5000 );
				dx57Hist->setTitle( "Hit Shift REF - Plane 5 in x;x_{7}-x_{5} [um];hit pairs" );

				dy57Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy57", 100, -5000, 5000 );
				dy57Hist->setTitle( "Hit Shift REF - Plane 5 in y;y_{7}-y_{5} [um];hit pairs" );

				dx67Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dx67", 100, -5000, 5000 );
				dx67Hist->setTitle( "Hit Shift REF - DUT in x;x_{7}-x_{6} [um];hit pairs" );

				dy67Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "InputHits/dy67", 100, -5000, 5000 );
				dy67Hist->setTitle( "Hit Shift REF - DUT in y;y_{7}-y_{6} [um];hit pairs" );
			}


			AIDAProcessor::tree(this)->mkdir("TripletsDriplets");

			trirxHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/trirx", 100, -1000, 1000 );
			trirxHist->setTitle( "Triplet Residual in x;x_{1}-x_{02} [um];triplets" );

			triryHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/triry", 100, -1000, 1000 );
			triryHist->setTitle( "Triplet Residual in y;y_{1}-y_{02} [um];triplets" );

			ntriHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/ntri", 21, -0.5, 20.5 );
			ntriHist->setTitle( "Number of Triplets;triplets;events" );


			trirxdutHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/tridutrx", 100, -1000, 1000 );
			trirxdutHist->setTitle( "Triplet DUT Extrapolation Residual in x;x_{6}-x_{02extrap} [um];triplets" );

			trirydutHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/tridutry", 100, -1000, 1000 );
			trirydutHist->setTitle( "Triplet DUT Extrapolation Residual in y;y_{6}-y_{02extrap} [um];triplets" );


			drirxHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/dridx", 100, -1000, 1000 );
			drirxHist->setTitle( "Driplet Residual in x;x_{4}-x_{35} [um];driplets" );

			driryHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/dridy", 100, -1000, 1000 );
			driryHist->setTitle( "Driplet Residual in y;y_{4}-y_{35} [um];driplets" );

			ndriHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/ndri", 21, -0.5, 20.5 );
			ndriHist->setTitle( "Number of Driplets;driplets;events" );

			if (_useREF == true)
			{

				drirxrefHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/drirefrx", 100, -1000, 1000 );
				drirxrefHist->setTitle( "Driplet REF Extrapolation Residual in x;x_{7}-x_{35extrap} [um];driplets" );

				driryrefHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/drirefry", 100, -1000, 1000 );
				driryrefHist->setTitle( "Driplet REF Extrapolation Residual in y;y_{7}-y_{35extrap} [um];driplets" );

			}

			sixrxHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/sixrx", 100, -1000, 1000 );
			sixrxHist->setTitle( "Tri-Driplet Residual in x;x_{A}-x_{B} [um];triplet/driplet pairs" );

			sixryHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/sixry", 100, -1000, 1000 );
			sixryHist->setTitle( "Tri-Driplet Residual in y;y_{A}-y_{B} [um];triplet/driplet pairs" );

			sixkxHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/sixkx", 100, -25, 25 );
			sixkxHist->setTitle( "Tri-Driplet Kink Angle in x;x kink angle [mrad];triplet/driplet pairs" );

			sixkyHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "TripletsDriplets/sixky", 100, -25, 25 );
			sixkyHist->setTitle( "Tri-Driplet Kink Angle in y;y kink angle [mrad];triplet/driplet pairs" );


			AIDAProcessor::tree(this)->mkdir("GBLInput");

			selxtHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selxt", 150, -15, 15 );
			selxtHist->setTitle( "Triplet Track x at Telescope Centre;x [mm];tracks" );

			selytHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selyt", 100, -10, 10 );
			selytHist->setTitle( "Triplet Track y at Telescope Centre;y [mm];tracks" );

			selaxtHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selaxt", 100, -25, 25 );
			selaxtHist->setTitle( "Triplet Track Angle in x;x angle [mrad];tracks" );

			selaytHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selayt", 100, -25, 25 );
			selaytHist->setTitle( "Triplet Track Angle in y;y angle [mrad];tracks" );

			selxdHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selxd", 150, -15, 15 );
			selxdHist->setTitle( "Driplet Track x at Telescope Centre;x [mm];tracks" );

			selydHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selyd", 100, -10, 10 );
			selydHist->setTitle( "Driplet Track y at Telescope Centre;y [mm];tracks" );

			selaxdHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selaxd", 100, -25, 25 );
			selaxdHist->setTitle( "Driplet Track Angle in x;x angle [mrad];tracks" );

			selaydHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selayd", 100, -25, 25 );
			selaydHist->setTitle( "Driplet Track Angle in y;y angle [mrad];tracks" );

			selrxHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selrx", 100, -5000, 5000 );
			selrxHist->setTitle( "Triplet-Driplet Track Match Residual in x;#Deltax [#mum];tracks" );

			selryHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selry", 100, -5000, 5000 );
			selryHist->setTitle( "Triplet-Driplet Track Match Residual in y;#Deltay [#mum];tracks" );

			selkxHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selkx", 100, -25, 25 );
			selkxHist->setTitle( "Triplet-Driplet Track Match Kink in x;kink x [mrad];tracks" );

			selkyHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selky", 100, -25, 25 );
			selkyHist->setTitle( "Triplet-Driplet Track Match Kink in y;kink y [mrad];tracks" );


			selrx0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selrx0", 100, -500, 500 );
			selrx0Hist->setTitle( "Triplet-Driplet Residual at Plane 0 in x;#Deltax [#mum];tracks" );

			selry0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selry0", 100, -500, 500 );
			selry0Hist->setTitle( "Triplet-Driplet Residual at Plane 0 in y;#Deltay [#mum];tracks" );

			selrx1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selrx1", 100, -500, 500 );
			selrx1Hist->setTitle( "Triplet-Driplet Residual at Plane 1 in x;#Deltax [#mum];tracks" );

			selry1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selry1", 100, -500, 500 );
			selry1Hist->setTitle( "Triplet-Driplet Residual at Plane 1 in y;#Deltay [#mum];tracks" );

			selrx2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selrx2", 100, -500, 500 );
			selrx2Hist->setTitle( "Triplet-Driplet Residual at Plane 2 in x;#Deltax [#mum];tracks" );

			selry2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selry2", 100, -500, 500 );
			selry2Hist->setTitle( "Triplet-Driplet Residual at Plane 2 in y;#Deltay [#mum];tracks" );

			selrx3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selrx3", 100, -500, 500 );
			selrx3Hist->setTitle( "Triplet-Driplet Residual at Plane 3 in x;#Deltax [#mum];tracks" );

			selry3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selry3", 100, -500, 500 );
			selry3Hist->setTitle( "Triplet-Driplet Residual at Plane 3 in y;#Deltay [#mum];tracks" );

			selrx4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selrx4", 100, -500, 500 );
			selrx4Hist->setTitle( "Triplet-Driplet Residual at Plane 4 in x;#Deltax [#mum];tracks" );

			selry4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selry4", 100, -500, 500 );
			selry4Hist->setTitle( "Triplet-Driplet Residual at Plane 4 in y;#Deltay [#mum];tracks" );

			selrx5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selrx5", 100, -500, 500 );
			selrx5Hist->setTitle( "Triplet-Driplet Residual at Plane 5 in x;#Deltax [#mum];tracks" );

			selry5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selry5", 100, -500, 500 );
			selry5Hist->setTitle( "Triplet-Driplet Residual at Plane 5 in y;#Deltay [#mum];tracks" );

			selrx6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selrx6", 100, -500, 500 );
			selrx6Hist->setTitle( "Triplet-Driplet Residual at the DUT in x;#Deltax [#mum];tracks" );

			selry6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selry6", 100, -500, 500 );
			selry6Hist->setTitle( "Triplet-Driplet Residual at the DUT in y;#Deltay [#mum];tracks" );

			if (_useREF == true)
			{
				selrx7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selrx7", 100, -500, 500 );
				selrx7Hist->setTitle( "Triplet-Driplet Residual at the REF in x;#Deltax [#mum];tracks" );

				selry7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLInput/selry7", 100, -500, 500 );
				selry7Hist->setTitle( "Triplet-Driplet Residual at the REF in y;#Deltay [#mum];tracks" );
			}


			AIDAProcessor::tree(this)->mkdir("GBLOutput");

			gblndfHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/gblndf", 16, -0.5, 15.5 );
			gblndfHist->setTitle( "GBL Fit NDF;GBL NDF;tracks" );

			gblchi2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/gblchi2", 1000, 0, 1000 );
			gblchi2Hist->setTitle( "GBL Fit chi2;GBL chi2;tracks" );

			gblchi2ndfHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/gblchi2ndf", 1000, 0, 1000 );
			gblchi2ndfHist->setTitle( "GBL Fit chi2/NDF;GBL chi2/NDF;tracks" );

			gblprbHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/gblprb", 1000, 0, 1 );
			gblprbHist->setTitle( "GBL Fit Probability;GBL fit probability;tracks" );


			AIDAProcessor::tree(this)->mkdir("GBLOutput/Bad");

			badxHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/badx", 150, -15, 15 );
			badxHist->setTitle( "x at DUT, bad GBL;x [mm];tracks" );

			badyHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/bady", 100, -10, 10 );
			badyHist->setTitle( "y at DUT, bad GBL;y [mm];tracks" );

			badaxHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/badax", 100, -25, 25 );
			badaxHist->setTitle( "track angle x, bad GBL;x angle [mrad];tracks" );

			badayHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baday", 100, -25, 25 );
			badayHist->setTitle( "track angle y, bad GBL;y angle [mrad];tracks" );

			baddxHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddx", 100, -5000, 5000 );
			baddxHist->setTitle( "track match x, bad GBL;#Deltax [#mum];tracks" );

			baddyHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddy", 100, -5000, 5000 );
			baddyHist->setTitle( "track match y, bad GBL;#Deltay [#mum];tracks" );

			badkxHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/badkx", 100, -25, 25 );
			badkxHist->setTitle( "kink x, bad GBL;kink x [mrad];tracks" );

			badkyHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/badky", 100, -25, 25 );
			badkyHist->setTitle( "kink y, bad GBL;kink y [mrad];tracks" );


			baddx0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddx0", 100, -500, 500 );
			baddx0Hist->setTitle( "triplet resid x at 0, bad GBL;#Deltax [#mum];tracks" );

			baddy0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddy0", 100, -500, 500 );
			baddy0Hist->setTitle( "triplet resid y at 0, bad GBL;#Deltay [#mum];tracks" );

			baddx1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddx1", 100, -500, 500 );
			baddx1Hist->setTitle( "triplet resid x at 1, bad GBL;#Deltax [#mum];tracks" );

			baddy1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddy1", 100, -500, 500 );
			baddy1Hist->setTitle( "triplet resid y at 1, bad GBL;#Deltay [#mum];tracks" );

			baddx2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddx2", 100, -500, 500 );
			baddx2Hist->setTitle( "triplet resid x at 2, bad GBL;#Deltax [#mum];tracks" );

			baddy2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddy2", 100, -500, 500 );
			baddy2Hist->setTitle( "triplet resid y at 2, bad GBL;#Deltay [#mum];tracks" );

			baddx3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddx3", 100, -500, 500 );
			baddx3Hist->setTitle( "triplet resid x at 3, bad GBL;#Deltax [#mum];tracks" );

			baddy3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddy3", 100, -500, 500 );
			baddy3Hist->setTitle( "triplet resid y at 3, bad GBL;#Deltay [#mum];tracks" );

			baddx4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddx4", 100, -500, 500 );
			baddx4Hist->setTitle( "triplet resid x at 4, bad GBL;#Deltax [#mum];tracks" );

			baddy4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddy4", 100, -500, 500 );
			baddy4Hist->setTitle( "triplet resid y at 4, bad GBL;#Deltay [#mum];tracks" );

			baddx5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddx5", 100, -500, 500 );
			baddx5Hist->setTitle( "triplet resid x at 5, bad GBL;#Deltax [#mum];tracks" );

			baddy5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddy5", 100, -500, 500 );
			baddy5Hist->setTitle( "triplet resid y at 5, bad GBL;#Deltay [#mum];tracks" );

			baddx6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddx6", 100, -500, 500 );
			baddx6Hist->setTitle( "triplet resid x at DUT, bad GBL;#Deltax [#mum];tracks" );

			baddy6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddy6", 100, -500, 500 );
			baddy6Hist->setTitle( "triplet resid y at DUT, bad GBL;#Deltay [#mum];tracks" );

			if (_useREF == true)
			{
				baddx7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddx7", 100, -500, 500 );
				baddx7Hist->setTitle( "triplet resid x at REF, bad GBL;#Deltax [#mum];tracks" );

				baddy7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Bad/baddy7", 100, -500, 500 );
				baddy7Hist->setTitle( "triplet resid y at REF, bad GBL;#Deltay [#mum];tracks" );
			}


			AIDAProcessor::tree(this)->mkdir("GBLOutput/Good");

			AIDAProcessor::tree(this)->mkdir("GBLOutput/Good/Plane0");

			AIDAProcessor::tree(this)->mkdir("GBLOutput/Good/Plane1");

			AIDAProcessor::tree(this)->mkdir("GBLOutput/Good/Plane2");

			AIDAProcessor::tree(this)->mkdir("GBLOutput/Good/Plane3");

			AIDAProcessor::tree(this)->mkdir("GBLOutput/Good/Plane4");

			AIDAProcessor::tree(this)->mkdir("GBLOutput/Good/Plane5");

			AIDAProcessor::tree(this)->mkdir("GBLOutput/Good/DUT");

			if (_useREF == true)
			{
				AIDAProcessor::tree(this)->mkdir("GBLOutput/Good/REF");
			}


			goodx0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane0/goodx0", 100, -500, 500 );
			goodx0Hist->setTitle( "triplet resid x at 0, good GBL;#Deltax [#mum];tracks" );

			goody0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane0/goody0", 100, -500, 500 );
			goody0Hist->setTitle( "triplet resid y at 0, good GBL;#Deltay [#mum];tracks" );

			goodx1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane1/goodx1", 100, -500, 500 );
			goodx1Hist->setTitle( "triplet resid x at 1, good GBL;#Deltax [#mum];tracks" );

			goody1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane1/goody1", 100, -500, 500 );
			goody1Hist->setTitle( "triplet resid y at 1, good GBL;#Deltay [#mum];tracks" );

			goodx2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane2/goodx2", 100, -500, 500 );
			goodx2Hist->setTitle( "triplet resid x at 2, good GBL;#Deltax [#mum];tracks" );

			goody2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane2/goody2", 100, -500, 500 );
			goody2Hist->setTitle( "triplet resid y at 2, good GBL;#Deltay [#mum];tracks" );

			goodx3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane3/goodx3", 100, -500, 500 );
			goodx3Hist->setTitle( "driplet resid x at 3, good GBL;#Deltax [#mum];tracks" );

			goody3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane3/goody3", 100, -500, 500 );
			goody3Hist->setTitle( "driplet resid y at 3, good GBL;#Deltay [#mum];tracks" );

			goodx4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane4/goodx4", 100, -500, 500 );
			goodx4Hist->setTitle( "driplet resid x at 4, good GBL;#Deltax [#mum];tracks" );

			goody4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane4/goody4", 100, -500, 500 );
			goody4Hist->setTitle( "driplet resid y at 4, good GBL;#Deltay [#mum];tracks" );

			goodx5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane5/goodx5", 100, -500, 500 );
			goodx5Hist->setTitle( "driplet resid x at 5, good GBL;#Deltax [#mum];tracks" );

			goody5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane5/goody5", 100, -500, 500 );
			goody5Hist->setTitle( "driplet resid y at 5, good GBL;#Deltay [#mum];tracks" );

			goodx6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/DUT/goodx6", 100, -500, 500 );
			goodx6Hist->setTitle( "triplet resid x at DUT, good GBL;#Deltax [#mum];tracks" );

			goody6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/DUT/goody6", 100, -500, 500 );
			goody6Hist->setTitle( "triplet resid y at DUT, good GBL;#Deltay [#mum];tracks" );

			if (_useREF == true)
			{
				goodx7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/REF/goodx7", 100, -500, 500 );
				goodx7Hist->setTitle( "driplet resid x at REF, good GBL;#Deltax [#mum];tracks" );

				goody7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/REF/goody7", 100, -500, 500 );
				goody7Hist->setTitle( "driplet resid y at REF, good GBL;#Deltay [#mum];tracks" );

			}


			dutrxxHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrxx", 100, -500, 500, 100, -15, 15 );
			dutrxxHist->setTitle( "Track Residual at DUT in x vs Track x;#Deltax [#mum];track_{x} [mm]" );

			dutrxyHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrxy", 100, -500, 500, 100, -15, 15 );
			dutrxyHist->setTitle( "Track Residual at DUT in x vs Track y;#Deltax [#mum];track_{y} [mm]" );

			dutrxzHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrxz", 100, -500, 500, 100, -15, 15 );
			dutrxzHist->setTitle( "Track Residual at DUT in x vs Track z;#Deltax [#mum];track_{z} [mm]" );


			dutryxHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutryx", 100, -500, 500, 100, -15, 15 );
			dutryxHist->setTitle( "Track Residual at DUT in y vs Track x;#Deltay [#mum];track_{x} [mm]" );

			dutryyHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutryy", 100, -500, 500, 100, -15, 15 );
			dutryyHist->setTitle( "Track Residual at DUT in y vs Track y;#Deltay [#mum];track_{y} [mm]" );

			dutryzHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutryz", 100, -500, 500, 100, -15, 15 );
			dutryzHist->setTitle( "Track Residual at DUT in y vs Track z;#Deltay [#mum];track_{z} [mm]" );


			dutrzxHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrzx", 100, -500, 500, 100, -15, 15 );
			dutrzxHist->setTitle( "Track Residual at DUT in z vs Track x;#Deltaz [#mum];track_{x} [mm]" );

			dutrzyHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrzy", 100, -500, 500, 100, -15, 15 );
			dutrzyHist->setTitle( "Track Residual at DUT in z vs Track y;#Deltaz [#mum];track_{y} [mm]" );

			dutrzzHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrzz", 100, -500, 500, 100, -15, 15 );
			dutrzzHist->setTitle( "Track Residual at DUT in z vs Track z;#Deltaz [#mum];track_{z} [mm]" );


			dutrxsxHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrxsx", 100, -500, 500, 100, -5.0, 5.0 );
			dutrxsxHist->setTitle( "Track Residual at DUT in x vs Track Slope in x;#Deltax [#mum];angle_{x} [mrad]" );

			dutrxsyHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrxsy", 100, -500, 500, 100, -5.0, 5.0 );
			dutrxsyHist->setTitle( "Track Residual at DUT in x vs Track Slope in y;#Deltax [#mum];angle_{y} [mrad]" );

			dutrysxHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrysx", 100, -500, 500, 100, -5.0, 5.0 );
			dutrysxHist->setTitle( "Track Residual at DUT in y vs Track Slope in x;#Deltay [#mum];angle_{x} [mrad]" );

			dutrysyHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrysy", 100, -500, 500, 100, -5.0, 5.0 );
			dutrysyHist->setTitle( "Track Residual at DUT in y vs Track Slope in y;#Deltay [#mum];angle_{y} [mrad]" );

			dutrzsxHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrzsx", 100, -500, 500, 100, -5.0, 5.0 );
			dutrzsxHist->setTitle( "Track Residual at DUT in z vs Track Slope in x;#Deltaz [#mum];angle_{x} [mrad]" );

			dutrzsyHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrzsy", 100, -500, 500, 100, -5.0, 5.0 );
			dutrzsyHist->setTitle( "Track Residual at DUT in z vs Track Slope in y;#Deltaz [#mum];angle_{y} [mrad]" );


			dutrxkxHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrxkx", 100, -500, 500, 100, -25, 25 );
			dutrxkxHist->setTitle( "Track Residual at DUT in x vs Kink Angle in x;#Deltax [#mum];angle_{x} [mrad]" );

			dutrxkyHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrxky", 100, -500, 500, 100, -25, 25 );
			dutrxkyHist->setTitle( "Track Residual at DUT in x vs Kink Angle in y;#Deltax [#mum];angle_{y} [mrad]" );

			dutrykxHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrykx", 100, -500, 500, 100, -25, 25 );
			dutrykxHist->setTitle( "Track Residual at DUT in y vs Kink Angle in x;#Deltay [#mum];angle_{x} [mrad]" );

			dutrykyHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutryky", 100, -500, 500, 100, -25, 25 );
			dutrykyHist->setTitle( "Track Residual at DUT in y vs Kink Angle in y;#Deltay [#mum];angle_{y} [mrad]" );

			dutrzkxHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrzkx", 100, -500, 500, 100, -25, 25 );
			dutrzkxHist->setTitle( "Track Residual at DUT in z vs Kink Angle in x;#Deltaz [#mum];angle_{x} [mrad]" );

			dutrzkyHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrzky", 100, -500, 500, 100, -25, 25 );
			dutrzkyHist->setTitle( "Track Residual at DUT in z vs Kink Angle in y;#Deltaz [#mum];angle_{y} [mrad]" );


			dutrxchi2ndfHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrxchi2ndf", 100, -500, 500, 100, 0, 50 );
			dutrxchi2ndfHist->setTitle( "Track Residual at DUT in x vs Track chi2/NDF;#Deltax [#mum];chi2/NDF" );

			dutrychi2ndfHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrychi2ndf", 100, -500, 500, 100, 0, 50 );
			dutrychi2ndfHist->setTitle( "Track Residual at DUT in y vs Track chi2/NDF;#Deltay [#mum];chi2/NDF" );

			dutrzchi2ndfHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrzchi2ndf", 100, -500, 500, 100, 0, 50 );
			dutrzchi2ndfHist->setTitle( "Track Residual at DUT in z vs Track chi2/NDF;#Deltaz [#mum];chi2/NDF" );


			dutrxprobHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrxprob", 100, -500, 500, 1000, 0, 1 );
			dutrxprobHist->setTitle( "Track Residual at DUT in x vs Track prob;#Deltax [#mum];prob" );

			dutryprobHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutryprob", 100, -500, 500, 1000, 0, 1 );
			dutryprobHist->setTitle( "Track Residual at DUT in y vs Track prob;#Deltay [#mum];prob" );

			dutrzprobHist = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/dutrzprob", 100, -500, 500, 1000, 0, 1 );
			dutrzprobHist->setTitle( "Track Residual at DUT in z vs Track prob;#Deltaz [#mum];prob" );


			dutkmap = AIDAProcessor::histogramFactory(this)->createProfile2D( "GBLOutput/Good/DUT/dutkmap", 1000, -15, 15, 1000, -15, 15, 0, 100 );
			dutkmap->setTitle("Track Kink at DUT;track_{x} [mm];track_{y} [mm];<kink^{2}> [mrad^{2}]");

			dutkxmap = AIDAProcessor::histogramFactory(this)->createProfile2D( "GBLOutput/Good/DUT/dutkxmap", 1000, -15, 15, 1000, -15, 15, 0, 100 );
			dutkxmap->setTitle("Track Kink in x at DUT;track_{x} [mm];track_{y} [mm];kink_{x} [mrad]");

			dutkymap = AIDAProcessor::histogramFactory(this)->createProfile2D( "GBLOutput/Good/DUT/dutkymap", 1000, -15, 15, 1000, -15, 15, 0, 100 );
			dutkymap->setTitle("Track Kink in y at DUT;track_{x} [mm];track_{y} [mm];kink_{y} [mrad]");


			duthitmap = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/duthitmap", 1000, -15, 15, 1000, -15, 15 );
			duthitmap->setTitle( "DUT Good Hits Map;hit_{x} [mm];hit_{y} [mm]" );

			duttrackhitmap = AIDAProcessor::histogramFactory(this)->createHistogram2D( "GBLOutput/Good/DUT/duttrackhitmap", 1000, -15, 15, 1000, -15, 15 );
			duttrackhitmap->setTitle( "Track Impact Point;track_{x} [mm];track_{y} [mm]" );


			// look at fit:
			gblax0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane0/gblax0", 100, -25, 25 );
			gblax0Hist->setTitle( "GBL angle at plane 0;x angle at plane 0 [mrad];tracks" );

			gbldx0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane0/gbldx0", 100, -500, 500 );
			gbldx0Hist->setTitle( "GBL shift at plane 0;x shift at plane 0 [#mum];tracks" );

			gblrx0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane0/gblrx0", 100, -250, 250 );
			gblrx0Hist->setTitle( "GBL resid at plane 0;x resid at plane 0 [#mum];tracks" );

			gblay0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane0/gblay0", 100, -25, 25 );
			gblay0Hist->setTitle( "GBL angle at plane 0;y angle at plane 0 [mrad];tracks" );

			gbldy0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane0/gbldy0", 100, -500, 500 );
			gbldy0Hist->setTitle( "GBL shift at plane 0;y shift at plane 0 [#mum];tracks" );

			gblry0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane0/gblry0", 100, -250, 250 );
			gblry0Hist->setTitle( "GBL resid at plane 0;y resid at plane 0 [#mum];tracks" );

			gblpx0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane0/gblpx0", 100, -10, 10 );
			gblpx0Hist->setTitle( "GBL x pull at plane 0;x pull at plane 0 [#sigma];tracks" );

			gblpy0Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane0/gblpy0", 100, -10, 10 );
			gblpy0Hist->setTitle( "GBL y pull at plane 0;y pull at plane 0 [#sigma];tracks" );


			gblax1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane1/gblax1", 100, -25, 25 );
			gblax1Hist->setTitle( "GBL angle at plane 1;x angle at plane 1 [mrad];tracks" );

			gbldx1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane1/gbldx1", 100, -500, 500 );
			gbldx1Hist->setTitle( "GBL shift at plane 1;x shift at plane 1 [#mum];tracks" );

			gblrx1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane1/gblrx1", 100, -250, 250 );
			gblrx1Hist->setTitle( "GBL resid at plane 1;x resid at plane 1 [#mum];tracks" );

			gblay1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane1/gblay1", 100, -25, 25 );
			gblay1Hist->setTitle( "GBL angle at plane 1;y angle at plane 1 [mrad];tracks" );

			gbldy1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane1/gbldy1", 100, -500, 500 );
			gbldy1Hist->setTitle( "GBL shift at plane 1;y shift at plane 1 [#mum];tracks" );

			gblry1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane1/gblry1", 100, -250, 250 );
			gblry1Hist->setTitle( "GBL resid at plane 1;y resid at plane 1 [#mum];tracks" );

			gblpx1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane1/gblpx1", 100, -10, 10 );
			gblpx1Hist->setTitle( "GBL x pull at plane 1;x pull at plane 1 [#sigma];tracks" );

			gblpy1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane1/gblpy1", 100, -10, 10 );
			gblpy1Hist->setTitle( "GBL y pull at plane 1;y pull at plane 1 [#sigma];tracks" );


			gblax2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane2/gblax2", 100, -25, 25 );
			gblax2Hist->setTitle( "GBL angle at plane 2;x angle at plane 2 [mrad];tracks" );

			gbldx2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane2/gbldx2", 100, -500, 500 );
			gbldx2Hist->setTitle( "GBL shift at plane 2;x shift at plane 2 [#mum];tracks" );

			gblrx2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane2/gblrx2", 100, -250, 250 );
			gblrx2Hist->setTitle( "GBL resid at plane 2;x resid at plane 2 [#mum];tracks" );

			gblay2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane2/gblay2", 100, -25, 25 );
			gblay2Hist->setTitle( "GBL angle at plane 2;y angle at plane 2 [mrad];tracks" );

			gbldy2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane2/gbldy2", 100, -500, 500 );
			gbldy2Hist->setTitle( "GBL shift at plane 2;y shift at plane 2 [#mum];tracks" );

			gblry2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane2/gblry2", 100, -250, 250 );
			gblry2Hist->setTitle( "GBL resid at plane 2;y resid at plane 2 [#mum];tracks" );

			gblpx2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane2/gblpx2", 100, -10, 10 );
			gblpx2Hist->setTitle( "GBL x pull at plane 2;x pull at plane 2 [#sigma];tracks" );

			gblpy2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane2/gblpy2", 100, -10, 10 );
			gblpy2Hist->setTitle( "GBL y pull at plane 2;y pull at plane 2 [#sigma];tracks" );


			gblax3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane3/gblax3", 100, -25, 25 );
			gblax3Hist->setTitle( "GBL angle at plane 3;x angle at plane 3 [mrad];tracks" );

			gbldx3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane3/gbldx3", 100, -500, 500 );
			gbldx3Hist->setTitle( "GBL shift at plane 3;x shift at plane 3 [#mum];tracks" );

			gblrx3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane3/gblrx3", 100, -250, 250 );
			gblrx3Hist->setTitle( "GBL resid at plane 3;x resid at plane 3 [#mum];tracks" );

			gblay3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane3/gblay3", 100, -25, 25 );
			gblay3Hist->setTitle( "GBL angle at plane 3;y angle at plane 3 [mrad];tracks" );

			gbldy3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane3/gbldy3", 100, -500, 500 );
			gbldy3Hist->setTitle( "GBL shift at plane 3;y shift at plane 3 [#mum];tracks" );

			gblry3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane3/gblry3", 100, -250, 250 );
			gblry3Hist->setTitle( "GBL resid at plane 3;y resid at plane 3 [#mum];tracks" );

			gblpx3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane3/gblpx3", 100, -10, 10 );
			gblpx3Hist->setTitle( "GBL x pull at plane 3;x pull at plane 3 [#sigma];tracks" );

			gblpy3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane3/gblpy3", 100, -10, 10 );
			gblpy3Hist->setTitle( "GBL y pull at plane 3;y pull at plane 3 [#sigma];tracks" );


			gblax4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane4/gblax4", 100, -25, 25 );
			gblax4Hist->setTitle( "GBL angle at plane 4;x angle at plane 4 [mrad];tracks" );

			gbldx4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane4/gbldx4", 100, -500, 500 );
			gbldx4Hist->setTitle( "GBL shift at plane 4;x shift at plane 4 [#mum];tracks" );

			gblrx4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane4/gblrx4", 100, -250, 250 );
			gblrx4Hist->setTitle( "GBL resid at plane 4;x resid at plane 4 [#mum];tracks" );

			gblay4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane4/gblay4", 100, -25, 25 );
			gblay4Hist->setTitle( "GBL angle at plane 4;y angle at plane 4 [mrad];tracks" );

			gbldy4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane4/gbldy4", 100, -500, 500 );
			gbldy4Hist->setTitle( "GBL shift at plane 4;y shift at plane 4 [#mum];tracks" );

			gblry4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane4/gblry4", 100, -250, 250 );
			gblry4Hist->setTitle( "GBL resid at plane 4;y resid at plane 4 [#mum];tracks" );

			gblpx4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane4/gblpx4", 100, -10, 10 );
			gblpx4Hist->setTitle( "GBL x pull at plane 4;x pull at plane 4 [#sigma];tracks" );

			gblpy4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane4/gblpy4", 100, -10, 10 );
			gblpy4Hist->setTitle( "GBL y pull at plane 4;y pull at plane 4 [#sigma];tracks" );


			gblax5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane5/gblax5", 100, -25, 25 );
			gblax5Hist->setTitle( "GBL angle at plane 5;x angle at plane 5 [mrad];tracks" );

			gbldx5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane5/gbldx5", 100, -500, 500 );
			gbldx5Hist->setTitle( "GBL shift at plane 5;x shift at plane 5 [#mum];tracks" );

			gblrx5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane5/gblrx5", 100, -250, 250 );
			gblrx5Hist->setTitle( "GBL resid at plane 5;x resid at plane 5 [#mum];tracks" );

			gblay5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane5/gblay5", 100, -25, 25 );
			gblay5Hist->setTitle( "GBL angle at plane 5;y angle at plane 5 [mrad];tracks" );

			gbldy5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane5/gbldy5", 100, -500, 500 );
			gbldy5Hist->setTitle( "GBL shift at plane 5;y shift at plane 5 [#mum];tracks" );

			gblry5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane5/gblry5", 100, -250, 250 );
			gblry5Hist->setTitle( "GBL resid at plane 5;y resid at plane 5 [#mum];tracks" );

			gblpx5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane5/gblpx5", 100, -10, 10 );
			gblpx5Hist->setTitle( "GBL x pull at plane 5;x pull at plane 5 [#sigma];tracks" );

			gblpy5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane5/gblpy5", 100, -10, 10 );
			gblpy5Hist->setTitle( "GBL y pull at plane 5;y pull at plane 5 [#sigma];tracks" );


			gblax6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/DUT/gblax6", 100, -25, 25 );
			gblax6Hist->setTitle( "GBL angle at DUT;x angle at DUT [mrad];tracks" );

			gbldx6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/DUT/gbldx6", 100, -500, 500 );
			gbldx6Hist->setTitle( "GBL shift at DUT;x shift at DUT [#mum];tracks" );

			gblrx6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/DUT/gblrx6", 100, -250, 250 );
			gblrx6Hist->setTitle( "GBL resid at DUT;x resid at DUT [#mum];tracks" );

			gblay6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/DUT/gblay6", 100, -25, 25 );
			gblay6Hist->setTitle( "GBL angle at DUT;y angle at DUT [mrad];tracks" );

			gbldy6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/DUT/gbldy6", 100, -500, 500 );
			gbldy6Hist->setTitle( "GBL shift at DUT;y shift at DUT [#mum];tracks" );

			gblry6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/DUT/gblry6", 100, -250, 250 );
			gblry6Hist->setTitle( "GBL resid at DUT;y resid at DUT [#mum];tracks" );

			gblpx6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/DUT/gblpx6", 100, -10, 10 );
			gblpx6Hist->setTitle( "GBL x pull at DUT;x pull at DUT [#sigma];tracks" );

			gblpy6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/DUT/gblpy6", 100, -10, 10 );
			gblpy6Hist->setTitle( "GBL y pull at DUT;y pull at DUT [#sigma];tracks" );


			if (_useREF == true)
			{
				gblax7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/REF/gblax7", 100, -25, 25 );
				gblax7Hist->setTitle( "GBL angle at REF;x angle at REF [mrad];tracks" );

				gbldx7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/REF/gbldx7", 100, -500, 500 );
				gbldx7Hist->setTitle( "GBL shift at REF;x shift at REF [#mum];tracks" );

				gblrx7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/REF/gblrx7", 100, -250, 250 );
				gblrx7Hist->setTitle( "GBL resid at REF;x resid at REF [#mum];tracks" );

				gblay7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/REF/gblay7", 100, -25, 25 );
				gblay7Hist->setTitle( "GBL angle at REF;y angle at REF [mrad];tracks" );

				gbldy7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/REF/gbldy7", 100, -500, 500 );
				gbldy7Hist->setTitle( "GBL shift at REF;y shift at REF [#mum];tracks" );

				gblry7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/REF/gblry7", 100, -250, 250 );
				gblry7Hist->setTitle( "GBL resid at REF;y resid at REF [#mum];tracks" );

				gblpx7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/REF/gblpx7", 100, -10, 10 );
				gblpx7Hist->setTitle( "GBL x pull at REF;x pull at REF [#sigma];tracks" );

				gblpy7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/REF/gblpy7", 100, -10, 10 );
				gblpy7Hist->setTitle( "GBL y pull at REF;y pull at REF [#sigma];tracks" );
			}


			gblkx1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane1/gblkx1", 100, -5, 5 );
			gblkx1Hist->setTitle( "GBL kink angle at plane 1;plane 1 kink [mrad];tracks" );

			gblkx2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane2/gblkx2", 100, -5, 5 );
			gblkx2Hist->setTitle( "GBL kink angle at plane 2;plane 2 kink [mrad];tracks" );

			gblkx3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane3/gblkx3", 100, -5, 5 );
			gblkx3Hist->setTitle( "GBL kink angle at plane 3;plane 3 kink [mrad];tracks" );

			gblkx4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane4/gblkx4", 100, -5, 5 );
			gblkx4Hist->setTitle( "GBL kink angle at plane 4;plane 4 kink [mrad];tracks" );

			gblkx5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane5/gblkx5", 100, -5, 5 );
			gblkx5Hist->setTitle( "GBL kink angle at plane 5;plane 5 kink [mrad];tracks" );

			gblkx6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/DUT/gblkx6", 100, -5, 5 );
			gblkx6Hist->setTitle( "GBL kink angle at plane 6;plane 6 kink [mrad];tracks" );

			if (_useREF == true)
			{
				gblkx7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/REF/gblkx7", 100, -5, 5 );
				gblkx7Hist->setTitle( "GBL kink angle at plane 7;plane 7 kink [mrad];tracks" );
			}


			gblky1Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane1/gblky1", 100, -5, 5 );
			gblky1Hist->setTitle( "GBL kink angle at plane 1;plane 1 kink [mrad];tracks" );

			gblky2Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane2/gblky2", 100, -5, 5 );
			gblky2Hist->setTitle( "GBL kink angle at plane 2;plane 2 kink [mrad];tracks" );

			gblky3Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane3/gblky3", 100, -5, 5 );
			gblky3Hist->setTitle( "GBL kink angle at plane 3;plane 3 kink [mrad];tracks" );

			gblky4Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane4/gblky4", 100, -5, 5 );
			gblky4Hist->setTitle( "GBL kink angle at plane 4;plane 4 kink [mrad];tracks" );

			gblky5Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/Plane5/gblky5", 100, -5, 5 );
			gblky5Hist->setTitle( "GBL kink angle at plane 5;plane 5 kink [mrad];tracks" );

			gblky6Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/DUT/gblky6", 100, -5, 5 );
			gblky6Hist->setTitle( "GBL kink angle at plane 6;plane 6 kink [mrad];tracks" );

			if (_useREF == true)
			{
				gblky7Hist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "GBLOutput/Good/REF/gblky7", 100, -5, 5 );
				gblky7Hist->setTitle( "GBL kink angle at plane 7;plane 7 kink [mrad];tracks" );

			}


			nmHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "nm", 21, -0.5, 20.5 );
			nmHist->setTitle( "Track Matches;track matches;events" );

			nmGoodHist = AIDAProcessor::histogramFactory(this)->createHistogram1D( "nmGood", 21, -0.5, 20.5 );
			nmGoodHist->setTitle( "Good Track Matches;good track matches;events" );


			tres01x = AIDAProcessor::histogramFactory(this)->createHistogram1D( "tres01x", 100, -250, 250 );
			tres01x->setTitle( "Fit Difference Method 0-1;#Deltax at DUT [#mum];tracks" );

			tres01y = AIDAProcessor::histogramFactory(this)->createHistogram1D( "tres01y", 100, -250, 250 );
			tres01y->setTitle( "Fit Difference Method 0-1;#Deltay at DUT [#mum];tracks" );

			tres01z = AIDAProcessor::histogramFactory(this)->createHistogram1D( "tres01z", 100, -250, 250 );
			tres01z->setTitle( "Fit Difference Method 0-1;#Deltaz at DUT [#mum];tracks" );


			tres02x = AIDAProcessor::histogramFactory(this)->createHistogram1D( "tres02x", 100, -250, 250 );
			tres02x->setTitle( "Fit Difference Method 0-2;#Deltax at DUT [#mum];tracks" );

			tres02y = AIDAProcessor::histogramFactory(this)->createHistogram1D( "tres02y", 100, -250, 250 );
			tres02y->setTitle( "Fit Difference Method 0-2;#Deltay at DUT [#mum];tracks" );

			tres02z = AIDAProcessor::histogramFactory(this)->createHistogram1D( "tres02z", 100, -250, 250 );
			tres02z->setTitle( "Fit Difference Method 0-2;#Deltaz at DUT [#mum];tracks" );


			tres12x = AIDAProcessor::histogramFactory(this)->createHistogram1D( "tres12x", 100, -250, 250 );
			tres12x->setTitle( "Fit Difference Method 1-2;#Deltax at DUT [#mum];tracks" );

			tres12y = AIDAProcessor::histogramFactory(this)->createHistogram1D( "tres12y", 100, -250, 250 );
			tres12y->setTitle( "Fit Difference Method 1-2;#Deltay at DUT [#mum];tracks" );

			tres12z = AIDAProcessor::histogramFactory(this)->createHistogram1D( "tres12z", 100, -250, 250 );
			tres12z->setTitle( "Fit Difference Method 1-2;#Deltaz at DUT [#mum];tracks" );

			if (_dthistos == true)
			{
				trackrate = AIDAProcessor::histogramFactory(this)->createHistogram2D( "trate", 1000, 0, 500000, 100, 0, 5.0 );
				trackrate->setTitle( "Track Rate;Event Number;tracks" );

				badtrackrate = AIDAProcessor::histogramFactory(this)->createHistogram2D( "btrate", 1000, 0, 500000, 100, 0, 5.0 );
				badtrackrate->setTitle( "Bad Track Rate;Event Number;tracks" );

				goodtrackrate = AIDAProcessor::histogramFactory(this)->createHistogram2D( "gtrate", 1000, 0, 500000, 100, 0, 1.0 );
				goodtrackrate->setTitle( "Good Track Rate;Event Number;tracks" );
			}

			dutrxxProf = new TProfile ("dutrxxProf","",150,-15,15,200,200,"s");
			_rootObjectMap.insert(make_pair("dutrxxProf", dutrxxProf));
			dutrxxProf->SetTitle("Track Residual at DUT in x vs Track x;track_{x} [mm];#Deltax [#mum]");

			dutrxyProf = new TProfile ("dutrxyProf","",150,-15,15,200,200,"s");
			_rootObjectMap.insert(make_pair("dutrxyProf", dutrxyProf));
			dutrxyProf->SetTitle("Track Residual at DUT in x vs Track y;track_{y} [mm];#Deltax [#mum]");

			dutrxzProf = new TProfile ("dutrxzProf","",150,-15,15,200,200,"s");
			_rootObjectMap.insert(make_pair("dutrxzProf", dutrxzProf));
			dutrxzProf->SetTitle("Track Residual at DUT in x vs Track z;track_{z} [mm];#Deltax [#mum]");

			dutryxProf = new TProfile ("dutryxProf","",150,-15,15,200,200,"s");
			_rootObjectMap.insert(make_pair("dutryxProf", dutryxProf));
			dutryxProf->SetTitle("Track Residual at DUT in y vs Track x;track_{x} [mm];#Deltay [#mum]");

			dutryyProf = new TProfile ("dutryyProf","",150,-15,15,200,200,"s");
			_rootObjectMap.insert(make_pair("dutryyProf", dutryyProf));
			dutryyProf->SetTitle("Track Residual at DUT in y vs Track y;track_{y} [mm];#Deltay [#mum]");

			dutryzProf = new TProfile ("dutryzProf","",150,-15,15,200,200,"s");
			_rootObjectMap.insert(make_pair("dutryzProf", dutryzProf));
			dutryzProf->SetTitle("Track Residual at DUT in y vs Track z;track_{z} [mm];#Deltay [#mum]");

			dutrzxProf = new TProfile ("dutrzxProf","",150,-15,15,200,200,"s");
			_rootObjectMap.insert(make_pair("dutrzxProf", dutrzxProf));
			dutrzxProf->SetTitle("Track Residual at DUT in z vs Track x;track_{x} [mm];#Deltaz [#mum]");

			dutrzyProf = new TProfile ("dutrzyProf","",150,-15,15,200,200,"s");
			_rootObjectMap.insert(make_pair("dutrzyProf", dutrzyProf));
			dutrzyProf->SetTitle("Track Residual at DUT in z vs Track y;track_{y} [mm];#Deltaz [#mum]");

			dutrzzProf = new TProfile ("dutrzzProf","",150,-15,15,200,200,"s");
			_rootObjectMap.insert(make_pair("dutrzzProf", dutrzzProf));
			dutrzzProf->SetTitle("Track Residual at DUT in z vs Track z;track_{z} [mm];#Deltaz [#mum]");

			profilefitalpha = new TF1("profilefitalpha","pol1");
			_rootObjectMap.insert(make_pair("profilefitalpha", profilefitalpha));

			profilefitbeta = new TF1("profilefitbeta","pol1");
			_rootObjectMap.insert(make_pair("profilefitbeta", profilefitbeta));

			profilefitgamma = new TF1("profilefitgamma","pol1");
			_rootObjectMap.insert(make_pair("profilefitgamma", profilefitgamma));


		} // done _doPreAlignment

	} //try
	catch( lcio::Exception& e )
	{

		#ifdef EUTEL_INTERACTIVE
		streamlog_out( ERROR2 ) << "No AIDAProcessor initialized. Type q to exit" << endl;
		string answer;
		while ( true )
		{
			streamlog_out( ERROR2 ) << "[q]" << endl;
			cin >> answer;
			transform( answer.begin(), answer.end(), answer.begin(), ::tolower );
			if( answer == "q" )
			{
				exit(-1);
			}
			break;
		}
		#else
		streamlog_out( WARNING2 ) << "No AIDAProcessor initialized. Continue without histogramming" << endl;
		#endif

	}
	#endif

}

#endif // USE_GEAR