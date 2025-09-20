#include <Util/DirectXUtils.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <ctime>

// Estructura para almacenar todos los datos del sol
struct SunData
{
    DirectX::XMFLOAT3 LightDirection;
    float SunSize;
    DirectX::XMFLOAT4 LightColor;
};

// Hemos añadido un nuevo parámetro: dayLengthMultiplier
inline SunData GetSunData(float latitude, const std::chrono::system_clock::time_point& currentTime, float dayLengthMultiplier)
{
    using namespace DirectX;
    using namespace std::chrono;

    SunData data;

    // --- Paso 1: Obtener el tiempo del día y el día del año ---
    const time_t current_t = system_clock::to_time_t(currentTime);

    tm timeinfo_data;
    const tm* timeinfo = &timeinfo_data;

    if (localtime_s(&timeinfo_data, &current_t) != 0)
    {
        return data;
    }

    float totalHoursInDay = (float)timeinfo->tm_hour + (float)timeinfo->tm_min / 60.0f + (float)timeinfo->tm_sec / 3600.0f;
    float hourAngle = (totalHoursInDay / 24.0f * XM_PI * 2.0f);

    float dayOfYear = (float)timeinfo->tm_yday;
    const float axialTilt = 23.45f * XM_PI / 180.0f;
    float solarDeclination = -axialTilt * cosf(dayOfYear / 365.0f * XM_PI * 2.0f);

    float latitudeRad = latitude * XM_PI / 180.0f;

    // --- Paso 2: Calcular la Altitud y el Acimut ---
    float sinAltitude = sinf(latitudeRad) * sinf(solarDeclination) + cosf(latitudeRad) * cosf(solarDeclination) * cosf(hourAngle);
    float altitudeAngle = asinf(sinAltitude);

    float cosAzimuth = (sinf(solarDeclination) - sinf(latitudeRad) * sinf(altitudeAngle)) / (cosf(latitudeRad) * cosf(altitudeAngle));
    float azimuthAngle = acosf(cosAzimuth);

    if (totalHoursInDay > 12.0f) {
        azimuthAngle = XM_PI * 2.0f - azimuthAngle;
    }

    // --- Paso 3: Convertir a vector de dirección 3D ---
    float sinAltitude_final = sinf(altitudeAngle);
    float cosAltitude_final = cosf(altitudeAngle);
    float sinAzimuth_final = sinf(azimuthAngle);
    float cosAzimuth_final = cosf(azimuthAngle);

    data.LightDirection.x = cosAltitude_final * sinAzimuth_final;
    data.LightDirection.y = sinAltitude_final;
    data.LightDirection.z = cosAltitude_final * cosAzimuth_final;

    // --- Paso 4: Calcular el Color y el Tamaño del Sol ---
    XMFLOAT4 sunsetColor = { 1.0f, 0.9f, 0.7f, 1.0f };
    XMFLOAT4 middayColor = { 1.0f, 1.0f, 0.9f, 1.0f };

    // Usamos el nuevo multiplicador para ajustar el factor de altitud
    float sunAltitudeFactor = std::clamp(altitudeAngle * dayLengthMultiplier, 0.0f, 1.0f);

    data.LightColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    //if (sunAltitudeFactor <= 0.7f)
    //{
    float smoothFactor = DirectXUtils::Smoothstep(0.0f, 1.0f, sunAltitudeFactor);
    XMStoreFloat4(&data.LightColor, XMVectorLerp(XMLoadFloat4(&sunsetColor), XMLoadFloat4(&middayColor), sunAltitudeFactor));
    //}

    data.SunSize = 0.5f + (1 / data.LightDirection.y);

    return data;
}
