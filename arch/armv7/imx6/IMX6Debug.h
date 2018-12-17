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

class IMX6Debug : public OutputStream {

public:
	static IMX6Debug pInstance;

	static IMX6Debug *startLine();
	static void endLine();
protected:
	void writeToDevice();

private:
#ifdef DEBUG_OUT_SYNC
	static Corelock pSyncLock;
#endif
};

#endif /* DEBUG_EXYNOSDEBUG_H_ */
