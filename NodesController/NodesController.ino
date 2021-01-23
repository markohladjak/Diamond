#include <IDevice.h>
#include <Node.h>
#include <Lift.h>
#include <LiftControlBox.h>

using namespace diamon;

#define NODE_ID 0

Node node(NODE_ID);

void setup()
{
	Lift lift;
	LiftControlBox liftControlBox(&lift);

	node.AddDevice(&lift);
}

void loop()
{
	node.Process();
}
