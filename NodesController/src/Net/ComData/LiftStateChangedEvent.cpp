#include <Net/ComData/LiftStateChangedEvent.h>
#include <Helpers/JsonHelper.h>


namespace diamon {

bool LiftStateChangedEvent::parse(JsonDocument &json) {
	if (!LiftEvent::parse(json))
		return false;

	State = LiftState::FromString(json[LIFT_MSG_TAG_STATE]);

	return true;
}

void LiftStateChangedEvent::serialize(JsonDocument &json) const {
	LiftEvent::serialize(json);

	json[LIFT_MSG_TAG_STATE] = State.ToString();
}

}
