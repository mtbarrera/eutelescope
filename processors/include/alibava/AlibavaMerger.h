/*
 * Created by Thomas Eichhorn
 *  (2014 DESY)
 *
 *  email:thomas.eichhorn@cern.ch
 */

#ifndef ALIBAVAMERGER_H
#define ALIBAVAMERGER_H 1

// alibava includes ".h"
#include "AlibavaBaseProcessor.h"

// marlin includes ".h"
#include "marlin/Processor.h"

// lcio includes <.h>
#include <IMPL/LCRunHeaderImpl.h>
#include <IMPL/TrackerRawDataImpl.h>

// ROOT includes <>
#include "TObject.h"

// system includes <>
#include <string>
#include <list>


namespace alibava {
	
	//! Alibava merge processor for Marlin.

	
	class AlibavaMerger:public alibava::AlibavaBaseProcessor   {
		
	public:
		
		
		//! Returns a new instance of AlibavaMerger
		/*! This method returns an new instance of the this processor.  It
		 *  is called by Marlin execution framework and it shouldn't be
		 *  called/used by the final user.
		 *
		 *  @return a new AlibavaMerger.
		 */
		virtual Processor * newProcessor () {
			return new AlibavaMerger;
		}
		
		//! Default constructor
		AlibavaMerger ();
		
		//! Called at the job beginning.
		/*! This is executed only once in the whole execution. It prints
		 *  out the processor parameters and reset all needed data
		 *  members. In principle this could also be the right place to
		 *  book histograms, but since those are also grouped according to
		 *  the detector numbers we need to have this parameter available.
		 */
		virtual void init ();
		
		//! Called for every run.
		/*! At the beginning of every run the run header is read and
		 *  processed by this method. As a first thing, the input run
		 *  header is dynamically re-casted to a EUTelRunHeaderImpl and
		 *  then important things like the number of detectors and the
		 *  pixel detector boundaries are dumped from the file. After that
		 *  the EUTelPedestalNoiseProcess::bookHistos() is called.
		 *
		 *  @param run the LCRunHeader of the this current run
		 */
		virtual void processRunHeader (LCRunHeader * run);
		
		//! Called every event
		/*! Since the behavior of the PedestalNoise processor is different
		 *  if this is the first or one of the following loop, this method
		 *  is just calling
		 *  AlibavaMerger::firstLoop(LCEvent*) or
		 *  AlibavaMerger::otherLoop(LCEvent*)
		 *
		 *  @param evt the current LCEvent event as passed by the
		 *  ProcessMgr
		 */
		virtual void processEvent (LCEvent * evt);
		
		
		//! Check event method
		/*! This method is called by the Marlin execution framework as
		 *  soon as the processEvent is over. It can be used to fill check
		 *  plots. For the time being there is nothing to check and do in
		 *  this slot.
		 *
		 *  @param evt The LCEvent event as passed by the ProcessMgr
		 *
		 */
		virtual void check (LCEvent * evt);
		
		
		//! Book histograms
		/*! This method is used to prepare the needed directory structure
		 *  within the current ITree folder and books all required
		 *  histograms. Histogram pointers are stored into
		 *  EUTelPedestalNoiseProcess::_rootObjectMap so that they can be
		 *  recalled and filled from anywhere in the code.  Apart from the
		 *  histograms listed in AlibavaMerger::fillHistos()
		 *  there is also a common mode histo described here below:
		 *
		 *  \li commonModeHisto: 1D histogram to store the calculated
		 *  common mode value for each event. This histogram is booked and
		 *  filled only if the loop counter is greater-equal than 1,
		 *  because for _iLoop == 0 there is no common mode suppression.
		 *  This histo is not filled with the other because it needs to be
		 *  updated every event.
		 *
		 *  @see AlibavaMerger::fillHistos() for the todos
		 */
		void bookHistos();
		
		//! Fill histograms
		/*! This method is used to fill in histograms for each channel. 
		 *
		 */
		void fillHistos();
		

		//! Called after data processing.
		/*! This method is called when the loop on events is finished. It
		 *  is checking whether the calculation is properly finished or
		 *  not.
		 *  A very common error occurs when the file finished without a
		 *  EORE or when the MaxRecordNumber was set to low to loop over
		 *  all the needed record. To check this is very easy because we
		 *  just have to crosscheck if _iLoop is equal to noOfCMIterations.
		 */
		virtual void end();

		//! The alibava file we want to read
		std::string _alibavaFile;

		//! The telescope file we want to read
		std::string _telescopeFile;

		//! The alibava collection name we want to read
		std::string _alibavaCollectionName;

		//! The telescope collection name
		std::string _telescopeCollectionName;

		//! The alibava collection name no2
		std::string _alibavaCollectionName2;

		//! The telescope collection name no2
		std::string _telescopeCollectionName2;

		//! The output collection names
		std::string _outputCollectionName;

		//! The output collection names no2
		std::string _outputCollectionName2;

		//! The output collection names no3
		std::string _outputCollectionName3;

		//! How do we want to output?
		int _outputmode;

		//! Move telescope sensor id?
		int _teleplaneshift;

		//! The reading instance
		LCReader* lcReader;

		//! The flag if the file is open
		bool _telescopeopen;

		//! The reading function
		LCEvent *readTelescope ();
		
		void addCorrelation(float ali_x, float ali_y, float ali_z, float tele_x, float tele_y, float tele_z, int event);
		
		//! The unsensitive axis of our strip sensor
		std::string _nonsensitiveaxis;
		
		//! Difference between the systems
		int _eventdifferenceTelescope;
		int _eventdifferenceAlibava;
		
		
	protected:
			
	
		
	};
	
	//! A global instance of the processor
	AlibavaMerger gAlibavaMerger;
	
}

#endif
