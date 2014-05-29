#include "blindtastic_models.h"
#include "svm_utils.h"
#include "blindtastic_concurrency.h"

void play_classify(const char* fvid, int once_every_x_frames, int reset_location_every_x_frames, bool reset_on_skip)
{
    const char* winp = "Detecting classifications live...";
    const char* track = "frame:";
    struct trackdata data;
    data.cap.open(fvid);
    if (!data.cap.isOpened()){
        return;
    }
    featureCallback last;
    ModelRepository m;
    LocationRepository locationRepository;
    SingleThreadExecutorService<CharacteristicValue> executor;
    std::vector<SVMCallable*> callables;
    int f = 0;
    cv::namedWindow(winp);
    cv::createTrackbar(track, winp, &f, getFrameCount(fvid),&trackbar_moved, &data);

    int frames_processed = 1;
    std::string topLocation = "Unknown";
    std::vector<std::string> detectedChars;
    while(data.cap.isOpened() && data.cap.read(data.img))
    {
        ++f;

        if (f % once_every_x_frames == 0)
        {
            int match = 0;
            last = NULL;

            std::map<featureCallback, CharacteristicDefinition>::const_iterator featpair;
            featpair = m.getCharacteristics().begin();

            while (featpair != m.getCharacteristics().end())
            {
                CharacteristicDefinition cdef = featpair->second;
                if (featpair->first == last)
                {
                    SVMCallable* callable = new SVMCallable(cdef,data.img,true);
                    callables.push_back(callable);
                    executor.submit(callable);
                }
                else
                {
                    SVMCallable* callable = new SVMCallable(cdef,data.img,false);
                    callables.push_back(callable);
                    executor.submit(callable);
                }

                while (executor.hasNextResult()) {
                    CharacteristicValue cv = executor.nextResult();
                    if (cv.weight > 0)
                    {
                        if(match == 0){
                            detectedChars.clear();
                        }
                        match++;
                        if(frames_processed % reset_location_every_x_frames == 0 || (reset_on_skip && data.skipped > 1)){
                            locationRepository.resetRefinement();
                        }
                        locationRepository.refine(cv);
                        topLocation = locationRepository.getTopLocation().first->getName() +
                                " : " + std::to_string(locationRepository.getTopLocation().second);
                        std::cout<<"Most likely location: "<<topLocation<<std::endl;
                        detectedChars.push_back(cv.definition->getName());
                    }
                }
                printText(data.img,topLocation, 400,600);
                for(size_t i = 0; i< detectedChars.size();i++){printText(data.img, detectedChars[i], 50, 75 + (35*(i+1)));}
                featpair++;
            }
            frames_processed++;
            cv::imshow(winp, data.img);
            int k = td::waitKey(100);
            if (k == K_ESC || k == K_Q)
                data.cap.release();
        }
        cv::setTrackbarPos(track, winp, f);
    }
    //release resources
    executor.interrupt(); //executor.shutdown(); finishes all tasks first
    for(auto callable : callables){
        delete callable;
    }
    if (data.cap.isOpened())
    {
        data.cap.release();
    }
    cv::destroyWindow(winp);
}



