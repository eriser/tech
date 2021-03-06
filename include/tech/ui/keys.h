#ifndef TECH_UI_KEYS_H
#define TECH_UI_KEYS_H


namespace Tech {


enum class KeyModifier {
	kNone    = 0x00,
	kShift   = 0x01,
	kControl = 0x02,
	kAlt     = 0x04,
	kMeta    = 0x08,
	kKeypad  = 0x10
};

using KeyModifiers = Flags<KeyModifier>;


enum class Key {
	kNoKey = 0x00,
	kReturn = 0x1004,

	kSpace = 0x20,
	kExclam = 0x21,
	kDoubleQoute = 0x22,
	kNumberSign = 0x23,
	kDollar = 0x24,
	kPercent = 0x25,
	kAmpersand = 0x26,
	kApostrophe = 0x27,
	kParenLeft = 0x28,
	kParenRight = 0x29,
	kAsterisk = 0x2A,
	kPlus = 0x2B,
	kComma = 0x2C,
	kMinus = 0x2D,
	kPeriod = 0x2E,
	kSlash = 0x2F,
	k0 = 0x30,
	k1 = 0x31,
	k2 = 0x32,
	k3 = 0x33,
	k4 = 0x34,
	k5 = 0x35,
	k6 = 0x36,
	k7 = 0x37,
	k8 = 0x38,
	k9 = 0x39,
	kColon = 0x3A,
	kSemicolon = 0x3B,
	kLess = 0x3C,
	kEqual = 0x3D,
	kGreater = 0x3E,
	kQuestion = 0x3F,
	kAt = 0x40,
	kA = 0x41,
	kB = 0x42,
	kC = 0x43,
	kD = 0x44,
	kE = 0x45,
	kF = 0x46,
	kG = 0x47,
	kH = 0x48,
	kI = 0x49,
	kJ = 0x4A,
	kK = 0x4B,
	kL = 0x4C,
	kM = 0x4D,
	kN = 0x4E,
	kO = 0x4F,
	kP = 0x50,
	kQ = 0x51,
	kR = 0x52,
	kS = 0x53,
	kT = 0x54,
	kU = 0x55,
	kV = 0x56,
	kW = 0x57,
	kX = 0x58,
	kY = 0x59,
	kZ = 0x5A,
	kBracketLeft = 0x5B,
	kBackslash = 0x5C,
	kBracketRight = 0x5D,
	kAsciiCircum = 0x5E,
	kUnderscore = 0x5F,
	kQuoteLeft = 0x60,

	kBraceLeft = 0x7B,
	kBar = 0x7C,
	kBraceRight = 0x7D,
	kAsciiTilde = 0x7E,
};


} // namespace Tech


#endif // TECH_UI_KEYS_H
