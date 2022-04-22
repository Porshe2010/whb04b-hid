#include <iostream>
#include <iterator>
#include <cerrno>
#include <cstdio>
#include <algorithm>
#include <iomanip>
#include <stdlib.h>
#include <math.h>

#include <hidapi.h>
#include <key.hxx>

//#define DEBUG

// Helper to output hex with iostreams
#define PADHEX(width, val) std::setfill('0') << std::setw(width) << std::hex << (unsigned)val

#define WHB04B_VENDOR 0x10CE
#define WHB04B_PRODUCT 0xEB93


#define RESET 0x01
#define STOP 0x02
#define START_PAUSE 0x03
#define FEED_UP 0x04
#define FEED_DOWN 0x05
#define SPINDLE_UP 0x06
#define SPINDLE_DOWN 0x07
#define M_HOME 0x08
#define SAFE_Z 0x09
#define W_HOME 0x0a
#define S_ON_OFF 0x0b
#define FN 0x0c
#define PROBE_Z 0x0d
#define CONTINUOUS 0x0e
#define STEP 0x0f

#define MACRO_1 0x04
#define MACRO_2 0x05
#define MACRO_3 0x06
#define MACRO_4 0x07
#define MACRO_5 0x08
#define MACRO_6 0x09
#define MACRO_7 0x0a
#define MACRO_8 0x0b
#define MACRO_9 0x0d
#define MACRO_10 0x10

#define D_AXIS_OFF 0x06
#define D_AXIS_X 0x11
#define D_AXIS_Y 0x12
#define D_AXIS_Z 0x13
#define D_AXIS_A 0x14

#define D_STEP_0_001 0x0d
#define D_STEP_0_01 0x0e
#define D_STEP_0_1 0x0f
#define D_STEP_1 0x10
#define D_STEP_60 0x1a
#define D_STEP_100 0x1b
#define D_STEP_LOAD 0x9b

#define CC_025 "1"
#define CC_25 "2"
#define CC_1 "3"
#define CC_FAST "4"

#define CC_X_UP "right"
#define CC_X_DOWN "left"
#define CC_Y_UP "up"
#define CC_Y_DOWN "down"
#define CC_Z_UP ","
#define CC_Z_DOWN "."


class WHB04B {
    public:
        WHB04B();
        ~WHB04B();

        enum class Axis {
            AXIS_NONE = 0xFF,
            AXIS_OFF = 0x06,
            AXIS_X = 0x11,
            AXIS_Y = 0x12,
            AXIS_Z = 0x13,
            AXIS_A = 0x14
        };

        enum class Step {
            STEP_NONE = 0xFF,
            STEP_0_001 = 0x0d,
            STEP_0_01 = 0x0e,
            STEP_0_1 = 0x0f,
            STEP_1 = 0x10,
            STEP_60 = 0x1a,
            STEP_100 = 0x1b,
            STEP_LOAD = 0x9b,
        };

        void updateAxis(Axis axis);
        void updateStep(Step step);
        void handleButtons(unsigned char key,unsigned char mod);
        void handleJog(char count);
        void handleHIDBuffer(unsigned char buf [], size_t len);
        const char *axisName(Axis axis);
        const char *stepName(Step step);
        const char *keyName(char key, char mod);

    private:
        Axis currAxis = Axis::AXIS_NONE;
        Step currStep = Step::STEP_NONE;
        const WonderRabbitProject::key::writer_t &writer;


#if 0
        int xhc_encode_float(float v, unsigned char *buf)
        {
            float abs_v = fabs(v);

            short int_part = (short)floor(abs_v);
            short fract_part = (short)(round((abs_v - int_part) * 10000.0f));
            if (v < 0) fract_part = fract_part | 0x8000;
            *(short *)buf = int_part;
            *((short *)buf+1) = fract_part;
            return 4;
        }

        int xhc_encode_s16(int v, unsigned char *buf)
        {
            *(short *)buf = v;
            return 2;
        }

        typedef struct {
            hal_float_t *x_wc, *y_wc, *z_wc, *a_wc;
            hal_float_t *x_mc, *y_mc, *z_mc, *a_mc;

            hal_float_t *feedrate_override, *feedrate;
            hal_float_t *spindle_override, *spindle_rps;

            hal_bit_t *button_pin[NB_MAX_BUTTONS];

            hal_bit_t *jog_enable_x;
            hal_bit_t *jog_enable_y;
            hal_bit_t *jog_enable_z;
            hal_bit_t *jog_enable_a;
            hal_bit_t *jog_enable_feedrate;
            hal_bit_t *jog_enable_spindle;
            hal_float_t *jog_scale;
            hal_s32_t *jog_counts, *jog_counts_neg;
        } xhc_hal_t;

        typedef struct {
            xhc_hal_t *hal;
            int step;
            xhc_axis_t axis;
        } xhc_t;



        void xhc_display_encode(xhc_t *xhc, unsigned char *data, int len)
        {
            unsigned char buf[6*7];
            unsigned char *p = buf;
            int i;

            assert(len == 6*8);

            memset(buf, 0, sizeof(buf));

            *p++ = 0xFE;
            *p++ = 0xFD;
            *p++ = 0x0C;

            if (xhc->axis == axis_a) 
            {
                p += xhc_encode_float(*(xhc->hal->a_wc), p);
            }
            else 
            {
                p += xhc_encode_float(*(xhc->hal->x_wc), p);
            }
            p += xhc_encode_float(*(xhc->hal->y_wc), p);
            p += xhc_encode_float(*(xhc->hal->z_wc), p);
            if (xhc->axis == axis_a) 
            {
                p += xhc_encode_float(*(xhc->hal->a_mc), p);
            }
            else 
            {
                p += xhc_encode_float(*(xhc->hal->x_mc), p);
            }
            p += xhc_encode_float(*(xhc->hal->y_mc), p);
            p += xhc_encode_float(*(xhc->hal->z_mc), p);

            p += xhc_encode_s16((int)100.0**(xhc->hal->feedrate_override), p);
            p += xhc_encode_s16((int)100.0**(xhc->hal->spindle_override), p);
            p += xhc_encode_s16((int)*(xhc->hal->feedrate), p);
            p += xhc_encode_s16(60 * (int)*(xhc->hal->spindle_rps), p);

            switch (xhc->step) {
                case 1:
                    buf[35] = 0x01;
                    break;
                case 10:
                    buf[35] = 0x03;
                    break;
                case 100:
                    buf[35] = 0x08;
                    break;
                case 1000:
                    buf[35] = 0x0A;
                    break;
            }

            // Multiplex to 6 USB transactions
            hid_write(handle,p,len);
        }

#endif
};
