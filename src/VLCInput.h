/* ------------------------------------------------------------------
 * Copyright (C) 2015 Matthias P. Braendli
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */

#ifndef __VLC_INPUT_H_
#define __VLC_INPUT_H_

#include "config.h"

#if HAVE_VLC

#include <cstdio>
#include <string>
#include <vector>
#include <deque>

#include <vlc/vlc.h>
#include <boost/thread/thread.hpp>

#include "SampleQueue.h"

// 16 bits per sample is fine for now
#define BYTES_PER_SAMPLE 2

class VLCInput
{
    public:
        VLCInput(const std::string& uri,
                 int rate,
                 unsigned verbosity) :
            m_uri(uri),
            m_verbosity(verbosity),
            m_channels(2),
            m_rate(rate),
            m_vlc(NULL) { }

        ~VLCInput() {
            if (m_mp) {
                /* Stop playing */
                libvlc_media_player_stop(m_mp);

                /* Free the media_player */
                libvlc_media_player_release(m_mp);
            }

            if (m_vlc) {
                libvlc_release(m_vlc);
                m_vlc = NULL;
            }
        }

        /* Prepare the audio input */
        int prepare();

        ssize_t read(uint8_t* buf, size_t length);


        // Callbacks for VLC
        void preRender(
                uint8_t** pp_pcm_buffer,
                size_t size);

        void postRender(
                uint8_t* p_pcm_buffer,
                size_t size);

        int getRate() { return m_rate; }

    protected:
        ssize_t m_read(uint8_t* buf, size_t length);

        std::vector<uint8_t> m_current_buf;

        boost::mutex m_queue_mutex;
        std::deque<uint8_t> m_queue;

        std::string m_uri;
        unsigned m_verbosity;
        unsigned m_channels;
        int m_rate;

        // VLC pointers
        libvlc_instance_t     *m_vlc;
        libvlc_media_player_t *m_mp;

    private:
        VLCInput(const VLCInput& other) {}
};

#endif // HAVE_VLC

#endif
