/*
 * ExynosDebug.h
 *
 *  Created on: 04.05.2015
 *      Author: hendrik
 */

#ifndef DEBUG_EXYNOSDEBUG_H_
#define DEBUG_EXYNOSDEBUG_H_

#include <common/OutputStream.h>

//#define DEBUG_OUT_SYNC
#ifdef DEBUG_OUT_SYNC
#include <sync/Corelock.h>
#endif

class OMAPDebug : public OutputStream {

public:
	static OMAPDebug pInstance;

	static OMAPDebug *startLine();
	static void endLine();
	void writeToDevice();
protected:

private:
#ifdef DEBUG_OUT_SYNC
	static Corelock pSyncLock;
#endif
};

#endif /* DEBUG_EXYNOSDEBUG_H_ */
