#include "blindtastic_models.h"
#include "svm_utils.h"
#include "blindtastic_concurrency.h"

void play_classify(const char* fvid, int once_every_x_frames, int reset_location_every_x_frames,bool reset_on_skip)
{
    const char* winp = "Detecting classifications live...";
    const char* track = "frame:";
    struct trackdata data;
    data.cap.open(fvid);
    if (!data.cap.isOpened()){
        return;
    }
    featureCallback last;
    ModelRepository modelRepository;
    LocationRepository locationRepository;
    std::string topLocation;
    int f = 0;
    cv::namedWindow(winp);
    cv::createTrackbar(track, winp, &f, getFrameCount(fvid),&trackbar_moved, &data);

    int frames_processed = 1;
    while(data.cap.isOpened() && data.cap.read(data.img))
    {
        ++f;

        if (f % once_every_x_frames == 0)
        {
            int match = 0;
            last = NULL;

            std::map<featureCallback, const CharacteristicDefinition*>::const_iterator featpair;
            featpair = modelRepository.getCharacteristics().begin();

            while (featpair != modelRepository.getCharacteristics().end())
            {
                const CharacteristicDefinition* cdef = featpair->second;
                double cval;
                if (featpair->first == last)
                {
                    cval = cdef->getValue(data.img, true).weight;
                }
                else
                {
                    cval = cdef->getValue(data.img, false).weight;
                    last = featpair->first;
                }

                if (cval > 0)
                {
                    if(frames_processed % reset_location_every_x_frames == 0 || (reset_on_skip && data.skipped > 1)){
                        locationRepository.resetRefinement(true);
                    }
                    CharacteristicValue cv = cdef->getValue(data.img, true);
                    locationRepository.refine(cv);
                    printText(data.img, cdef->getName(), 50, 75 + (35*(match++)));
                }

                featpair++;
            }

            topLocation = locationRepository.getTopLocation().first->getName();
            topLocation += " : ";
            topLocation += std::to_string(locationRepository.getTopLocation().second);
            printText(data.img,topLocation, 400,600);

            frames_processed++;
            cv::imshow(winp, data.img);
            int k = td::waitKey(10);
            if (k == K_ESC || k == K_Q)
                data.cap.release();
        }
        cv::setTrackbarPos(track, winp, f);
    }

    if (data.cap.isOpened())
    {
        data.cap.release();
    }
    cv::destroyWindow(winp);
}

void play_classify_mt(const char* fvid, int reset_location_every_x_frames, bool reset_on_skip)
{
    const char* window_name = "Detecting classifications live...";
    cv::namedWindow(window_name);

    const char* track = "frame:";
    int current_frame_nr = 0;
    struct trackdata data;
    data.cap.open(fvid);
    if (!data.cap.isOpened()){
        return;
    }
    cv::createTrackbar(track, window_name, &current_frame_nr, getFrameCount(fvid),&trackbar_moved, &data);

    ModelRepository modelRepository;
    LocationRepository locationRepository;
    SingleThreadExecutorService<std::vector<CharacteristicValue>> executor;
    BundledCallable<CharacteristicValue> svmtask;

    int frames_processed = 0;
    int svm_frame_nr = 0;
    std::vector<std::string> detectedChars;
    std::string topLocation = "Unknown";

    while(data.cap.isOpened() && data.cap.read(data.img))
    {
        ++current_frame_nr;

        if (executor.hasNextResult() || executor.isIdle())
        {
            /****** update current results *****/

            if (executor.hasNextResult()) //can be false on frame nr 1
            {
                detectedChars.clear();
                detectedChars.push_back(std::string("results of frame ")+std::to_string(svm_frame_nr));
                std::vector<CharacteristicValue> cvals = executor.nextResult();
                for (std::vector<CharacteristicValue>::const_iterator cv = cvals.begin() ; cv != cvals.end() ; cv++)
                {
                    if (cv->weight > 0)
                    {
                        if(frames_processed % reset_location_every_x_frames == 0 || (reset_on_skip && data.skipped > 1))
                        {
                            locationRepository.resetRefinement(true);
                        }
                        locationRepository.refine(*cv);
                        detectedChars.push_back(cv->definition->getName());
                    }
                }
            }

            /****** prepare current frame to be processed *****/

            svmtask.clear();
            svm_frame_nr = current_frame_nr;
            featureCallback last = NULL;
            std::map<featureCallback, const CharacteristicDefinition*>::const_iterator feature_pair;
            feature_pair = modelRepository.getCharacteristics().begin();

            while (feature_pair != modelRepository.getCharacteristics().end())
            {
                const CharacteristicDefinition* cdef = feature_pair->second;
                if (feature_pair->first == last)
                {
                    svmtask.addCallable(new SVMCallable(cdef,data.img,true));
                }
                else
                {
                    svmtask.addCallable(new SVMCallable(cdef,data.img,false));
                    last = feature_pair->first;
                }

                feature_pair++;
            }
            executor.submit(&svmtask);

            ++frames_processed;
        }

        //print (last) results on image

        if (detectedChars.size() > 1)
        {
            topLocation = locationRepository.getTopLocation().first->getName();
            topLocation += " : ";
            topLocation += std::to_string(locationRepository.getTopLocation().second);
        }
        printText(data.img,topLocation, 400,600);

        for(size_t i = 0; i< detectedChars.size();i++)
        {
            printText(data.img, detectedChars[i], 50, 75 + (35*i));
        }

        //show image

        cv::setTrackbarPos(track, window_name, current_frame_nr);
        cv::imshow(window_name, data.img);
        int k = td::waitKey(100);
        if (k == K_ESC || k == K_Q)
            data.cap.release();
    }
    //release resources
    executor.interrupt(); //executor.shutdown(); finishes all tasks first
    if (data.cap.isOpened())
    {
        data.cap.release();
    }
    cv::destroyWindow(window_name);
}



