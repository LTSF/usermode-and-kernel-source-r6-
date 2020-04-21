namespace math 
{
#define M_PI	3.14159265358979323846264338327950288419716939937510
	namespace skeleton
	{
		D3DXVECTOR3 GetEntityHead(uint64_t player)
		{
			if (!player)
				return D3DXVECTOR3();

			uint64_t pSkeleton = read<uint64_t>(player + offsets::skeleton::entity_skeleton);

			if (!pSkeleton)
				return D3DXVECTOR3();

			return read<D3DXVECTOR3>(pSkeleton + offsets::skeleton::skeleton_head);
		}

		D3DXVECTOR3 GetEntityPos(uint64_t player)
		{
			if (!player)
				return D3DXVECTOR3();

			uint64_t pSkeleton = read<uint64_t>(player + offsets::skeleton::entity_skeleton);

			if (!pSkeleton)
				return D3DXVECTOR3();

			return read<D3DXVECTOR3>(pSkeleton + offsets::skeleton::skeleton_pos);
		}
	}

	D3DXVECTOR3 GetViewRight()
	{
		if (!offsets::managers::camera_manager)
			return D3DXVECTOR3();

		return read<D3DXVECTOR3>(offsets::managers::camera_manager + offsets::camera::camera_right);
	}

	D3DXVECTOR3 GetViewUp()
	{
		if (!offsets::managers::camera_manager)
			return D3DXVECTOR3();

		return read<D3DXVECTOR3>(offsets::managers::camera_manager + offsets::camera::camera_up);
	}

	D3DXVECTOR3 GetViewForward()
	{
		if (!offsets::managers::camera_manager)
			return D3DXVECTOR3();

		return read<D3DXVECTOR3>(offsets::managers::camera_manager + offsets::camera::camera_forward);
	}

	D3DXVECTOR3 GetViewTranslation()
	{
		if (!offsets::managers::camera_manager)
			return D3DXVECTOR3();

		return read<D3DXVECTOR3>(offsets::managers::camera_manager + offsets::camera::camera_translation);
	}

	float GetViewFovX()
	{
		if (!offsets::managers::camera_manager)
			return 0.f;

		return read<float>(offsets::managers::camera_manager + offsets::camera::camera_fovx);
	}

	float GetViewFovY()
	{
		if (!offsets::managers::camera_manager)
			return 0.f;

		return read<float>(offsets::managers::camera_manager + offsets::camera::camera_fovy);
	}

	inline float dot(D3DXVECTOR3 a, D3DXVECTOR3 b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	float WorldToScreen(D3DXVECTOR3 position, D3DXVECTOR2* Screen)
	{
		D3DXVECTOR3 temp = position - GetViewTranslation();

		D3DXVECTOR3 up = read<D3DXVECTOR3>(offsets::camera::camera_manager + offsets::camera::camera_up);

		float x = dot(temp, GetViewRight());
		float y = dot(temp, GetViewUp());
		float z = dot(temp, GetViewForward() * -1.f);

		if (z <= 0)
			return false;

		Screen->x = (Width / 2.f) * (1.f + x / GetViewFovX() / z);
		Screen->y = (Height / 2.f) * (1.f - y / GetViewFovY() / z);

		return z >= 1.0f ? true : false;
	}
	
	namespace aimbot
	{
		inline float Length(D3DXVECTOR3 dir)
		{
			return sqrt((dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z));
		}

		void clamp(D3DXVECTOR3 angle)
		{
			
			if (angle.x > 75.f) angle.x = 75.f;
			else if (angle.x < -75.f) angle.x = -75.f;
			if (angle.z < -180) angle.z += 360.0f;
			else if (angle.z > 180) angle.z -= 360.0f;

			angle.y = 0.f;
		}

		D3DXVECTOR3 CalcAngle(D3DXVECTOR3 enemy, D3DXVECTOR3 local)
		{
			auto direction = enemy - local;

			auto x = std::asin(direction.z / Length(direction)) * 57.2957795131f;
			auto y = std::atan(direction.y / direction.x) * 57.2957795131f;

			if (direction.x >= 0.f) y += 180.f;
			if (x > 180.f) x -= 360.f;
			else if (x < -180.f) x += 360.f;

			auto angle = D3DXVECTOR3(x, 0.f, y + 90.f);
			clamp(angle);

			return angle;
		}
		D3DXVECTOR3 VectorAngles(D3DXVECTOR3 forward)
		{
			D3DXVECTOR3 angles;
			float tmp, yaw, pitch;

			if (forward.y == 0 && forward.y == 0)
			{
				yaw = 0;
				if (forward.y > 0)
					pitch = 270;
				else
					pitch = 90;
			}
			else
			{
				yaw = (float)(atan2(forward.y, forward.x) * 180 / M_PI);


				tmp = (float)sqrt(forward.x * forward.x + forward.y * forward.y);
				pitch = (float)(atan2(forward.z * -1, tmp) * 180 / M_PI);

			}

			yaw += 90;

			if (yaw > 180)
			{
				yaw -= 360;
			}
			if (pitch > 180)
			{
				pitch -= 360;
			}

			angles.x = pitch;
			angles.y = yaw;
			angles.z = 0;
			return angles;
		}
		static D3DXVECTOR3 ToEuler(D3DXVECTOR4 q)
		{
			D3DXVECTOR3 end = D3DXVECTOR3();

			float sinr = (float)(+2.0 * (q.w * q.x + q.y * q.z));
			float cosr = (float)(+1.0 - 2.0 * (q.x * q.x + q.y * q.y));
			end.z = (float)atan2(sinr, cosr);

			double sinp = +2.0 * (q.w * q.y - q.z * q.x);
			if (abs(sinp) >= 1)
				end.x = (float)copysign(M_PI / 2, sinp);
			else
				end.x = (float)asin(sinp);

			double siny = +2.0 * (q.w * q.z + q.x * q.y);
			double cosy = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
			end.y = (float)atan2(siny, cosy);

			return end;
		}
		static float GetDifference(float firstAngle, float secondAngle)
		{
			float difference = secondAngle - firstAngle;
			while (difference < -180.f) difference += 360.f;
			while (difference > 180.f) difference -= 360.f;
			return difference;
		}
		D3DXVECTOR4 calculate_quaternion(float yaw, float pitch, float roll)
		{
			D3DXVECTOR4 q;
			double cy = cos(yaw * 0.5);
			double sy = sin(yaw * 0.5);
			double cr = cos(pitch * 0.5);
			double sr = sin(pitch * 0.5);
			double cp = cos(roll * 0.5);
			double sp = sin(roll * 0.5);

			q.w = (float)(cy * cr * cp + sy * sr * sp);
			q.x = (float)(cy * sr * cp - sy * cr * sp);
			q.y = (float)(cy * cr * sp + sy * sr * cp);
			q.z = (float)(sy * cr * cp - cy * sr * sp);
			return q;
		}
		D3DXVECTOR4 GetAngle(D3DXVECTOR3 viewTranslation, D3DXVECTOR3 enemyHead, D3DXVECTOR4 originalAngle)
		{
			D3DXVECTOR3 aimAngle = VectorAngles(viewTranslation - enemyHead);
			D3DXVECTOR3 currentAngle = ToEuler(originalAngle);
			currentAngle *= (float)(180.f / M_PI);
			D3DXVECTOR3 currentAngleV = D3DXVECTOR3(currentAngle.z, currentAngle.y, currentAngle.x);

			D3DXVECTOR3 smoothedAngle = D3DXVECTOR3(aimAngle.x, aimAngle.y, aimAngle.z);
			smoothedAngle.x = GetDifference(currentAngleV.x, smoothedAngle.x);
			smoothedAngle.y = GetDifference(currentAngleV.y, smoothedAngle.y);
			smoothedAngle /= (settings::aimbot::aimspeed);
			currentAngleV.x += smoothedAngle.x;
			currentAngleV.y += smoothedAngle.y;

			if (currentAngleV.x > 89.0f && currentAngleV.x <= 180.0f)
			{
				currentAngleV.x = 89.0f;
			}
			while (currentAngleV.x > 180.f)
			{
				currentAngleV.x -= 360.f;
			}
			while (currentAngleV.x < -89.0f)
			{
				currentAngleV.x = -89.0f;
			}
			while (currentAngleV.y > 180.f)
			{
				currentAngleV.y -= 360.f;
			}
			while (currentAngleV.y < -180.f)
			{
				currentAngleV.y += 360.f;
			}

			aimAngle *= (float)(M_PI / 180.f);
			currentAngle *= (float)(M_PI / 180.f);
			currentAngleV *= (float)(M_PI / 180.f);

			D3DXVECTOR4 newQuaternion = calculate_quaternion(currentAngleV.y, currentAngleV.x, 0);
			return newQuaternion;
		}

		void set_viewangle(std::uintptr_t entity, std::uintptr_t offset, D3DXVECTOR4 value)
		{
			auto r1 = read<uintptr_t>(entity + 0x20);
			auto r2 = read<uintptr_t>(r1 + offsets::camera::view_angles);

			write<D3DXVECTOR4>(r2 + offset, value);
		}

		D3DXVECTOR4 get_viewangle(uintptr_t entity, uintptr_t offset)
		{
			auto r1 = read<uintptr_t>(entity + 0x20);
			auto r2 = read<uintptr_t>(r1 + offsets::camera::view_angles);
			return read<D3DXVECTOR4>(r2 + offset);
		};

		static float ScreenToEnemy(D3DXVECTOR3 position)
		{
			D3DXVECTOR2 out;
			WorldToScreen(position, &out);
			return (fabs(out.x - (Width / 2)) + fabs(out.y - (Height / 2)));
		}
	}
}