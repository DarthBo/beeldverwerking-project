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
    ModelRepository m;
    LocationRepository locationRepository;
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

            std::map<featureCallback, CharacteristicDefinition>::const_iterator featpair;
            featpair = m.getCharacteristics().begin();

            while (featpair != m.getCharacteristics().end())
            {
                CharacteristicDefinition cdef = featpair->second;
                double cval;
                if (featpair->first == last)
                {
                    cval = cdef.getValue(data.img, true).weight;
                }
                else
                {
                    cval = cdef.getValue(data.img, false).weight;
                    last = featpair->first;
                }

                if (cval > 0)
                {
                    if(frames_processed % reset_location_every_x_frames == 0 || (reset_on_skip && data.skipped > 1)){
                        locationRepository.resetRefinement();
                    }
                    CharacteristicValue cv = cdef.getValue(data.img, true);
                    locationRepository.refine(cv);
                    std::string topLocation = locationRepository.getTopLocation().first->getName() +
                            " : " + std::to_string(locationRepository.getTopLocation().second);
                    printText(data.img,topLocation, 400,600);
                    printText(data.img, cdef.getName(), 50, 75 + (35*(match++)));
                }

                featpair++;
            }
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


void play_classify_in_background(const char* fvid, int once_every_x_frames, int reset_location_every_x_frames,bool reset_on_skip)
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
    std::vector<std::future<CharacteristicValue>> futures;
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

            std::map<featureCallback, CharacteristicDefinition>::const_iterator featpair;
            featpair = m.getCharacteristics().begin();

            while (featpair != m.getCharacteristics().end())
            {
                CharacteristicDefinition cdef = featpair->second;
                double cval;
                if (featpair->first == last)
                {
                    SVMCallable callable(cdef,data.img,true);
                    auto future = executor.submit(callable);
                    futures.push_back(std::move(future));
                }
                else
                {
                    SVMCallable callable(cdef,data.img,false);
                    auto future = executor.submit(callable);
                    futures.push_back(std::move(future));
                    last = featpair->first;
                }

                for(auto &future : futures){
                    if(is_ready(future)){
                        cval = future.get().weight;
                        if (cval > 0)
                        {
                            if(frames_processed % reset_location_every_x_frames == 0 || (reset_on_skip && data.skipped > 1)){
                                locationRepository.resetRefinement();
                            }
                            CharacteristicValue cv = cdef.getValue(data.img, true);
                            locationRepository.refine(cv);
                            std::string topLocation = locationRepository.getTopLocation().first->getName() +
                                    " : " + std::to_string(locationRepository.getTopLocation().second);
                            printText(data.img,topLocation, 400,600);
                            printText(data.img, cdef.getName(), 50, 75 + (35*(match++)));
                        }
                    }
                }

                featpair++;
            }
            frames_processed++;
            cv::imshow(winp, data.img);
            int k = td::waitKey(10);
            if (k == K_ESC || k == K_Q)
                data.cap.release();
        }
        cv::setTrackbarPos(track, winp, f);
    }
    //wait for final futures, show on screen?
    executor.shutdown();
    if (data.cap.isOpened())
    {
        data.cap.release();
    }
    cv::destroyWindow(winp);
}


