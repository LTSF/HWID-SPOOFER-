#pragma once

#include <algorithm>

namespace engine::structures
{
	enum bones : int
	{
		root = 13,

		l_thigh_1 = 15,
		l_thigh_2 = 16,

		r_thigh_1 = 20,
		r_thigh_2 = 21,

		l_calf = 17,
		r_calf = 22,

		l_foot = 18,
		r_foot = 23,

		pelvis = 14,

		spine_1 = 29,
		spine_2 = 36,
		spine_3 = 37,

		l_collarbone = 89,
		r_collarbone = 110,

		l_upperarm = 111,
		r_upperarm = 90,

		l_forearm_1 = 91,
		l_forearm_2 = 92,
		l_forearm_3 = 93,

		r_forearm_1 = 112,
		r_forearm_2 = 113,
		r_forearm_3 = 114,

		head = 133,
	};

	enum engine_bones : int
	{
		Invalid = -1,
		Empty = 0,
		IKController,
		Mesh,
		Vest_0,
		Vest_1,
		backpack,
		backpack_0,
		backpack_1,
		backpack_2,
		razgruz,
		razgruz_0,
		razgruz_1,
		razgruz_2,
		Root_Joint,
		HumanPelvis,
		HumanLThigh1,
		HumanLThigh2,
		HumanLCalf,
		HumanLFoot,
		HumanLToe,
		HumanRThigh1,
		HumanRThigh2,
		HumanRCalf,
		HumanRFoot,
		HumanRToe,
		Bear_Feet,
		USEC_Feet,
		BEAR_feet_1,
		weapon_holster_pistol,
		HumanSpine1,
		HumanGear1,
		HumanGear2,
		HumanGear3,
		HumanGear4,
		HumanGear4_1,
		HumanGear5,
		HumanSpine2,
		HumanSpine3,
		IK_S_LForearm1,
		IK_S_LForearm2,
		IK_S_LForearm3,
		IK_S_LPalm,
		IK_S_LDigit11,
		IK_S_LDigit12,
		IK_S_LDigit13,
		IK_S_LDigit21,
		IK_S_LDigit22,
		IK_S_LDigit23,
		IK_S_LDigit31,
		IK_S_LDigit32,
		IK_S_LDigit33,
		IK_S_LDigit41,
		IK_S_LDigit42,
		IK_S_LDigit43,
		IK_S_LDigit51,
		IK_S_LDigit52,
		IK_S_LDigit53,
		XYZ,
		LCollarbone_anim,
		RCollarbone_anim,
		RCollarbone_anim_XYZ,
		Weapon_root_3rd_anim,
		Weapon_root_3rd_anim_XYZ_1,
		Bend_Goal_Left,
		Bend_Goal_Right,
		Bend_Goal_Right_XYZ_1,
		HumanRibcage,
		IK_LForearm1,
		IK_LForearm2,
		IK_LForearm3,
		IK_LPalm,
		IK_LDigit11,
		IK_LDigit12,
		IK_LDigit13,
		IK_LDigit21,
		IK_LDigit22,
		IK_LDigit23,
		IK_LDigit31,
		IK_LDigit32,
		IK_LDigit33,
		IK_LDigit41,
		IK_LDigit42,
		IK_LDigit43,
		IK_LDigit51,
		IK_LDigit52,
		IK_LDigit53,
		Camera_animated,
		CameraContainer,
		Cam,
		HumanLCollarbone,
		HumanLUpperarm,
		HumanLForearm1,
		HumanLForearm2,
		HumanLForearm3,
		HumanLPalm,
		HumanLDigit11,
		HumanLDigit12,
		HumanLDigit13,
		HumanLDigit21,
		HumanLDigit22,
		HumanLDigit23,
		HumanLDigit31,
		HumanLDigit32,
		HumanLDigit33,
		HumanLDigit41,
		HumanLDigit42,
		HumanLDigit43,
		HumanLDigit51,
		HumanLDigit52,
		HumanLDigit53,
		HumanRCollarbone,
		HumanRUpperarm,
		HumanRForearm1,
		HumanRForearm2,
		HumanRForearm3,
		HumanRPalm,
		HumanRDigit11,
		HumanRDigit12,
		HumanRDigit13,
		HumanRDigit21,
		HumanRDigit22,
		HumanRDigit23,
		HumanRDigit31,
		HumanRDigit32,
		HumanRDigit33,
		HumanRDigit41,
		HumanRDigit42,
		HumanRDigit43,
		HumanRDigit51,
		HumanRDigit52,
		HumanRDigit53,
		Weapon_root,
		HumanNeck,
		HumanHead,
		HumanBackpack,
		weapon_holster,
		weapon_holster1,
		Camera_animated_3rd
	};

	struct rotator_t
	{
		float x;
		float y;
		float z;

		rotator_t clamp()
		{
			x = std::clamp(remainderf(x, 360.0f), -89.0f, 89.0f);
			y = std::clamp(remainderf(y, 360.0f), -180.0f, 180.0f);

			return { x, y };
		}

		rotator_t normalize()
		{
			if (x > 180)
				x -= 360.0f;
			if (x < 180)
				x += 360.0f;
			if (y > 180)
				y -= 360.0f;
			if (y < 180)
				y += 360.0f;

			return { x, y };
		}

		const rotator_t operator*(float value) const
		{
			return { x * value, y * value, z * value };
		}

		rotator_t operator-(const rotator_t& rot) const
		{
			return { x - rot.x, y - rot.y, z - rot.z };
		}

		rotator_t operator+(const rotator_t& rot) const
		{
			return { x + rot.x, y + rot.y, z + rot.z };
		}

		rotator_t operator/(const float& f) const
		{
			return { x / f, y / f, z / f };
		}
	};

	struct quat_t
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct vector2_t
	{
		float x;
		float y;

		vector2_t operator-(vector2_t value)
		{
			return { x - value.x, y - value.y };
		}

		vector2_t operator+(vector2_t value)
		{
			return { x + value.x, y + value.y };
		}

		vector2_t operator*(float value)
		{
			return { x * value, y * value };
		}

		vector2_t operator/(float value)
		{
			return { x / value, y / value };
		}

		vector2_t operator/=(float value)
		{
			x /= value;
			y /= value;

			return *this;
		}

		vector2_t operator+=(vector2_t value)
		{
			return { x += value.x, y += value.y };
		}

		vector2_t operator-=(vector2_t value)
		{
			return { x -= value.x, y -= value.y };
		}

		const float dot(const vector2_t& vec) const
		{
			return x * vec.x + y * vec.y;
		}
	};

	struct vector3_t
	{
		float x;
		float y;
		float z;

		const float dot(const vector3_t& vec) const
		{
			return x * vec.x + y * vec.y + z * vec.z;
		}

		const float distance(vector3_t vector) const
		{
			return sqrtf(powf(vector.x - x, 2.0) + powf(vector.y - y, 2.0) + powf(vector.z - z, 2.0));
		}

		const vector3_t operator*(float value) const
		{
			return { x * value, y * value };
		}

		const float squared() const
		{
			return x * x + y * y + z * z;
		}

		vector3_t operator-(const vector3_t& vec) const
		{
			return { x - vec.x, y - vec.y, z - vec.z };
		}
	};

	struct vector4_t
	{
		float x;
		float y;
		float z;
		float w;

		float color[4] = { x, y, z, w };

		const float dot(const vector3_t& vec) const
		{
			return x * vec.x + y * vec.y + z * vec.z;
		}

		vector3_t operator-(const vector3_t& vec) const
		{
			return { x - vec.x, y - vec.y, z - vec.z };
		}
	};

	struct matrix3x4_t
	{
		union
		{
			float matrix3x4[3][4];
		};
	};

	struct matrix4x4_t
	{
		union
		{
			float matrix4x4[4][4];
		};
	};

	struct transform_t
	{
		quat_t rotation;
		vector3_t translation;
		uint32_t pad;
		vector3_t scale_3d;
	};
}