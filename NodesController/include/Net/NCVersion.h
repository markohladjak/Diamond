#ifndef NCVERSION_H_
#define NCVERSION_H_

namespace diamon {

class NCVersion {
public:

	uint8_t major = 0;
	uint8_t minor = 0;
	uint8_t build = 0;
	uint16_t revision = 0;

	NCVersion() {}

	static NCVersion FromString(String version)
	{
		uint16_t v[4];
		int vp = 0;
		String s;

		for (int i = 0; i < version.length(); ++i)
		{
			if (version.c_str()[i] != '.')
				s += version.c_str()[i];
			else {
				v[vp++] = s.toInt();
				s = "";
			}
		}

		v[vp++] = s.toInt();
		s = "";

		NCVersion ver;

		ver.major = v[0];
		ver.minor = v[1];
		ver.build = v[2];
		ver.revision = v[3];

		return ver;
	}

	String ToString() { return String(major) + "." + String(minor) + "." + String(build) + "." + String(revision); }
};

}

#endif
