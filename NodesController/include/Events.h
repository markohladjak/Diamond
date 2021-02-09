/*
 * Events.h
 *
 *  Created on: Jan 26, 2021
 *      Author: rem
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include <list>
#include <Arduino.h>

namespace diamon {

template<class ...TParams>
class AbstractEventHandler
{
    public:
		virtual ~AbstractEventHandler() {}

        virtual void call( TParams... params ) = 0;
    protected:
        AbstractEventHandler() {}
};

template<class ...TParams>
class TEvent
{
    using TEventHandler = AbstractEventHandler<TParams...>;
    public:
        TEvent() :
            m_handlers()
        {
        }
        ~TEvent()
        {
        	for( TEventHandler* oneHandler : m_handlers )
                delete oneHandler;
            m_handlers.clear();
        }
        void operator()( TParams... params )
        {
            for( TEventHandler* oneHandler : m_handlers )
                oneHandler->call( params... );
        }
        void operator+=( TEventHandler& eventHandler )
        {
            m_handlers.push_back( &eventHandler );
        }
    private:
        std::list<TEventHandler*> m_handlers;
};

template<class TObject, class ...TParams>
class MethodEventHandler : public AbstractEventHandler<TParams...>
{
    using TMethod = void( TObject::* )( TParams... );
    public:
        MethodEventHandler( TObject& object, TMethod method ) :
            AbstractEventHandler<TParams...>(),
            m_object( object ),
            m_method( method )
        {
            assert( m_method != nullptr );
        }
        virtual void call( TParams... params ) override final
        {
            ( m_object.*m_method )( params... );
        }
    private:
        TObject& m_object;
        TMethod m_method;
};

template<class TObject, class ...TParams>
AbstractEventHandler<TParams...>& createMethodEventHandler( TObject& object, void( TObject::*method )( TParams... ) )
{
    return *new MethodEventHandler<TObject, TParams...>( object, method );
}

#define METHOD_HANDLER_EX( Object, Method ) createMethodEventHandler( Object, &Method )
#define METHOD_HANDLER( Method ) METHOD_HANDLER_EX( *this, Method )

} /* namespace diamon */

#endif /* EVENTS_H_ */
