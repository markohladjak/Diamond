/*
 * IInLoop.h
 *
 *  Created on: Jan 25, 2021
 *      Author: rem
 */

#ifndef INCLUDE_IINLOOP_H_
#define INCLUDE_IINLOOP_H_

namespace diamon
{

class IInLoop
{
public:
	virtual ~IInLoop() {}

	virtual void update() = 0;
};


}



#endif /* INCLUDE_IINLOOP_H_ */
