#include "blindtastic_models.h"
#include "svm_utils.h"

void play_classify(const char* fvid, int once_every_x_frames)
{
    const char* winp = "Detecting classifications live...";
    const char* track = "frame:";
    struct trackdata data;
    data.cap.open(fvid);

    featureCallback last;
    ModelRepository m;

    int f = 0;

    cv::namedWindow(winp);
    cv::createTrackbar(track, winp, &f, getFrameCount(fvid),&trackbar_moved, &data);

    while(data.cap.isOpened() && data.cap.read(data.img))
    {
        ++f;

        if (f%once_every_x_frames == 0)
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
                    printText(data.img, cdef.getName(), 50, 75 + (35*(match++)));
                }

                featpair++;
            }

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

