#include "IM920s.h"
#include "mbed.h"
#include <mbed_wait_api.h>


#include "IM920s.h"

void IM920::setReset (bool flg) {
    if (_reset) {
        if (flg) {
            _reset->output();
            _reset->write(0);
        } else {
            _reset->input();
            _reset->mode(PullNone);
        }
    }
}

void IM920::isrUart () {
    recvData(getUart());
}

int IM920::getUart () {
    char c;
    return _im.read(&c,1);
}

void IM920::putUart (char c) {
    _im.write(&c,sizeof(c));
}

int IM920::lockUart (int ms) {
    Timer t;

    if (_busy && _busy->read()) {
        // CTS check
        t.start();
        while (_busy->read()) {
            if (t.read_ms() >= ms) {
                DBG("cts timeout\r\n");
                return -1;
            }
        }
    }
    return 0;
}

void IM920::unlockUart () {
}

void IM920::initUart (PinName busy, PinName reset, int baud) {
    _baud = baud;
    if (_baud) _im.baud(_baud);
    _im.attach(Callback<void()>(this, &IM920::isrUart), UnbufferedSerial::RxIrq);

    _busy = NULL;
    _reset = NULL;
    if (busy != NC) {
        _busy = new DigitalIn(busy);
    }
    if (reset != NC) {
        _reset = new DigitalInOut(reset);
    }
}
char IM920::i2x (int i) {
    if (i >= 0 && i <= 9) {
        return i + '0';
    } else
    if (i >= 10 && i <= 15) {
        return i - 10 + 'A';
    }
    return 0;
}
int IM920::x2i (char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;
}

void IM920::clearFlags () {
    _state.ok = false;
    _state.failure = false;
    _state.res = RES_NULL;
    _state.n = 0;
}

void IM920::recvData (char c) {
    static int sub, len, count;
    static char chr;

#ifdef DEBUG_DUMP
    if (c < 0x20 || c >= 0x7f) {
        std::printf("_%02x", c);
    } else {
        std::printf("_%c", c);
    }
#endif
    switch (_state.mode) {
    case MODE_COMMAND:
        switch (c) {
        case 0:
        case 0x0a: // LF
        case 0x0d: // CR
            _state.buf[len] = 0;
            len = 0;
            parseMessage();
            break;
        case ':':
            if (_state.buf[2] == ',' && _state.buf[7] == ',' && len == 10) {
                sub = 0;
                _state.mode = MODE_DATA_RX;
                break;
            }
            /* FALLTHROUGH */
        default:
            if (len < sizeof(_state.buf) - 1) {
                _state.buf[len] = c;
                len ++;
            }
            break;
        }
        break;

    case MODE_DATA_RX:
        if (c == '\r' || c == '\n') {
            DBG("recv %d/%d\r\n", count, len);
            _state.received = true;
            _state.mode = MODE_COMMAND;
            len = 0;
            break;
        }
        switch (sub) {
        case 0:
            chr = x2i(c) << 4;
            sub ++;
            break;
        case 1:
            chr |= x2i(c);
            sub ++;
            if (_state.data!= NULL) {
                _state.data->queue(chr);
                if (_state.data->available() >= CFG_DATA_SIZE) {
                    _state.received = true;
                    WARN("buf full");
                }
            }
            count ++;
            break;
        case 2:
            if (c == ',') {
                sub = 0;
            }
            break;
        }
    }
}



#define RES_TABLE_NUM 4
int IM920::parseMessage () {
    int i;
    static const struct RES_TABLE {
        const Response res;
        void (IM920::*func)(const char*);
    } res_table[RES_TABLE_NUM] = {
      {RES_NULL,        NULL},
      {RES_RDID,        &IM920::resRDID},
      {RES_RDNN,        &IM920::resRDNN},
      {RES_RDRS,        &IM920::resRDRS},
    };

    if (_state.res != RES_NULL) {
      for (i = 0; i < RES_TABLE_NUM; i ++) {
        if (res_table[i].res == _state.res) {
            DBG("parse res %d '%s'\r\n", i, _state.buf);
            if (res_table[i].func != NULL) {
                (this->*(res_table[i].func))(_state.buf);
            }
        }
      }
    }

    if (strncmp(_state.buf, "OK", 2) == 0) {
        _state.ok = true;
        if (_state.status == STAT_SLEEP) {
            _state.status = STAT_NONE;
        }
        return 0;
    } else
    if (strncmp(_state.buf, "NG", 2) == 0) {
        _state.failure = true;
        return 0;
    }

    return -1;
}

void IM920::resRDID (const char *buf) {

    if (buf[0] < '0' || buf[0] > 'F') return;

    _state.id = strtol(buf, NULL, 16);
    _state.res = RES_NULL;
}

void IM920::resRDNN (const char *buf) {

    if (buf[0] < '0' || buf[0] > 'F') return;

    _state.node = strtol(buf, NULL, 16);
    _state.res = RES_NULL;
}

void IM920::resRDRS (const char *buf) {

    if (buf[0] < '0' || buf[0] > 'F') return;

    _state.rssi = strtol(buf, NULL, 16);
    _state.res = RES_NULL;
}


IM920::IM920 (PinName tx, PinName rx, PinName busy, PinName reset, int baud) : _im(tx, rx) {

    memset(&_state, 0, sizeof(_state));
    _state.data = new CircBuffer<char>(CFG_DATA_SIZE);

    initUart(busy, reset, baud);
    setReset(true);
    wait_us(100);
    setReset(false);
}

/*int IM920::init () {

    cmdRDID();
    cmdRDNN();
    cmdSTPO(3);  // 10dBm
    cmdSTRT(2);  // 1.25kbps
    return 0;
}*/

/*void IM920::poll () {

    if (_state.received && _state.buf != NULL)
      if (!_state.data->isEmpty()) {
        _func.call();
        if (_state.data->isEmpty()) {
            _state.received = false;
        }
    }
}*/
int IM920::sendData(const char * data, int len, int timeout) {
    int i;
    Timer t;

    if (lockUart(timeout)) return -1;

    if (len > 64) len = 64;
    clearFlags();
    putUart('T');
    putUart('X');
    putUart('D');
    putUart('A');
    putUart(' ');
    for (i = 0; i < len; i ++) {
        putUart(i2x((data[i]>>4) & 0x0f));
        putUart(i2x(data[i] & 0x0f));
    }
    putUart('\r');
    putUart('\n');
    unlockUart();
    INFO("data: TXDA %d\r\n", len);

    if (timeout) {
        t.start();
        for (;;) {
            if (_state.ok) break;
            if (_state.failure || t.read_ms() > timeout) {
                WARN("failure or timeout\r\n");
                return -1;
            }
        }
        t.stop();
    }

    return i;
}



int IM920::send (char *buf, int len) {

    if (len > 64) len = 64;

    return sendData(buf, len);
}


