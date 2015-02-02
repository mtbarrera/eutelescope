// LCIO
#include <EVENT/LCCollection.h>
#include "lcio.h"

// MARLIN
#include "marlin/Exceptions.h"
#include "marlin/Global.h"
#include "marlin/Processor.h"
#include "marlin/VerbosityLevels.h"

//EUTelescope
#include "EUTelUtility.h"
#include "EUTelEventImpl.h"
#include "EUTelHistogram.h"

namespace eutelescope {

	class  EUTelProcessorPlotTrack : public marlin::Processor {

  	private:
  	DISALLOW_COPY_AND_ASSIGN(EUTelProcessorPlotTrack)
        
    public:

    virtual Processor* newProcessor() {
    	return new  EUTelProcessorPlotTrack;
    }

    EUTelProcessorPlotTrack();

    /** Called at the begin of the job before anything is read.
    * Use to initialize the processor, e.g. book histograms.
    */
    virtual void init();

    /** Called for every run.*/
    virtual void processRunHeader(LCRunHeader* run);

    /** Called for every event - the working horse.*/
    virtual void processEvent(LCEvent * evt);

    virtual void check(LCEvent * evt);

   	/** Called after data processing for clean up. **/
		virtual void end();


		protected:

    /** Input TrackerHit collection name */
    std::string _trackCandidatesInputCollectionName;

   /** Output Tracks collection name */
   std::string _tracksOutputCollectionName;

   /** Histogram info file name */
	std::string _histoInfoFileName;



	};

    EUTelProcessorPlotTrack gEUTelProcessorPlotTrack;

}
