#include "Generator.h"
#include <qdebug.h>
#include <QtGlobal>
#include <QtCore/QtEndian>
#include <QtCore/QTime>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SECONDS     1
#define SYSTEM_FREQ 44100

Generator::Generator(float secs, int freq)
    :QIODevice( )
{
    m_freq = freq;
    fillData(m_freq, secs); /* mono FREQHz sine */
}

Generator::Generator(Generator *copyFrom)
    : QIODevice(), buffer(copyFrom->buffer)
{
    m_freq = copyFrom->m_freq;
    pos = 0;
    finished = false;
    trailing = copyFrom->trailing;
}

Generator::~Generator()
{
}

void Generator::clearBuffer() {
    buffer.clear();
    trailing = 0;
    pos = 0;
}

void Generator::appendDataFrom(const Generator *copyFrom) {
    // merge our trailing and the other data, but never merge to the past.
    int offs = qMax(buffer.size() - trailing, pos);
    int after_trailing = qMin(buffer.size() - offs, copyFrom->buffer.size());
    for (int i = 0; i < after_trailing; ++i) {
	buffer[offs + i] = qToLittleEndian(
	    qFromLittleEndian(buffer[offs + i]) +
	    qFromLittleEndian(copyFrom->buffer[i]));
    }

    // append the rest of the other data
    buffer << copyFrom->buffer.mid(after_trailing);

    trailing = qMax(copyFrom->trailing,
		    buffer.size() - offs - (copyFrom->buffer.size() - copyFrom->trailing));
    if (copyFrom->buffer.size() > 0)
	finished = false;
}

void Generator::start()
{
    open(QIODevice::ReadOnly);
}

void Generator::stop()
{
    close();
}

void Generator::fillData(int frequency, float seconds)
{
    int i;
    int value;
    int signal_samples = int(seconds*SYSTEM_FREQ);
    double slope = 4e-3;	// 4ms nominal slope time

    // The envelope wave form extends past the beginning and the end.
    // 3 times the slope length is a good estimate for a bound (~0.0002).
    double extra_time = 3 * slope;
    int extra_samples = extra_time * SYSTEM_FREQ;

    int total_samples = signal_samples + 2 * extra_samples;
    for(i=0; i<total_samples; i++) {
        if (frequency == 0.0)
            value = 0;
        else
            value=(int)(32767.0*sin(2.0*M_PI*((double)(i))*(double)(frequency)/SYSTEM_FREQ));

        // use an optimized envelope, according to
        // http://fermi.la.asu.edu/w9cf/articles/click/index.html

	// shift the envelope so that we start at 0, scale to right erf dimension
	double fi = (double)i / SYSTEM_FREQ - extra_time;
    	double filter = 0.5 * (erf(fi/slope) -
    			       erf((fi - seconds)/slope));

    	value *= filter;

	buffer << qToLittleEndian(value);
    }

    trailing = extra_samples * 2;
    finished = false;

#define ADD_SILENCE 100e-3	// add 100ms trailing silence
#if defined(ADD_SILENCE)
    for (i = 0; i < ADD_SILENCE * SYSTEM_FREQ; ++i) {
	buffer << 0;
	trailing++;
    }
#endif
}

void Generator::restartData()
{
    pos = 0;
}

qint64 Generator::readData(char *data, qint64 maxlen)
{
    int len = maxlen;
    if (len > 65536)
        len = 65536;

    const qint16 *src = &buffer.constData()[pos];

    len /= sizeof(*src);

    if (finished)
	return -1;

    if (pos >= buffer.size()) {
	finished = true;
        return 0;
    }

    finished = false;
    len = qMin(len, buffer.size() - pos);

    qint64 byte_len = len * sizeof(*src);
    memcpy(data, src, len * sizeof(*src));
    pos += len;

    return byte_len;
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

QTime Generator::timeLeft()
{
    int samples_left = buffer.size() - pos;

    int secs = samples_left / SYSTEM_FREQ;
    int msec = samples_left % SYSTEM_FREQ * 1000 / SYSTEM_FREQ;
    return QTime(0, 0, secs, msec);
}
