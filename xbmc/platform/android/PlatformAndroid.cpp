/*
 *  Copyright (C) 2016-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "PlatformAndroid.h"

#include "ServiceBroker.h"
#include "filesystem/SpecialProtocol.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/log.h"
#include "windowing/android/WinSystemAndroidGLESContext.h"

#include "platform/android/activity/XBMCApp.h"
#include "platform/android/powermanagement/AndroidPowerSyscall.h"

#include <stdlib.h>

#include <androidjni/Build.h>
#include <androidjni/PackageManager.h>

CPlatform* CPlatform::CreateInstance()
{
  return new CPlatformAndroid();
}

bool CPlatformAndroid::InitStageOne()
{
  if (!CPlatformPosix::InitStageOne())
    return false;
  setenv("SSL_CERT_FILE", CSpecialProtocol::TranslatePath("special://xbmc/system/certs/cacert.pem").c_str(), 1);

  setenv("OS", "Linux", true); // for python scripts that check the OS

  setenv("JAVA_HOME", CSpecialProtocol::TranslatePath("special://xbmcbin/../../../cache/lib/j2re-image/").c_str(), 1);

  setenv("JDK_HOME", CSpecialProtocol::TranslatePath("special://xbmcbin/../../../cache/lib/j2re-image/").c_str(), 1);

  setenv("LIBBLURAY_CP", CSpecialProtocol::TranslatePath("special://xbmcbin/../../../cache/lib/j2re-image/").c_str(), 1);

  std::string cache_path = "-Djava.io.tmpdir=" ;

  std::string kodi_path = cache_path + CSpecialProtocol::TranslatePath("special://xbmcbin/../../../cache/lib/j2re-image/");

  setenv("_JAVA_OPTIONS", kodi_path.c_str(), 1);

  setenv("LIBBLURAY_CACHE_ROOT", CSpecialProtocol::TranslatePath("special://userdata/cache/bluray/cache/").c_str(), 1);

  setenv("LIBBLURAY_PERSISTENT_ROOT", CSpecialProtocol::TranslatePath("special://userdata/cache/bluray/cache/").c_str(), 1);

  CWinSystemAndroidGLESContext::Register();

  CAndroidPowerSyscall::Register();

  return true;
}

bool CPlatformAndroid::InitStageThree()
{
  if (!CPlatformPosix::InitStageThree())
    return false;

  if (CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_guiVideoLayoutTransparent)
  {
    CLog::Log(LOGINFO, "XBMCApp: VideoLayout view was set to transparent.");
    CXBMCApp::Get().SetVideoLayoutBackgroundColor(0);
  }

  return true;
}

void CPlatformAndroid::PlatformSyslog()
{
  CLog::Log(
      LOGINFO,
      "Product: {}, Device: {}, Board: {} - Manufacturer: {}, Brand: {}, Model: {}, Hardware: {}",
      CJNIBuild::PRODUCT, CJNIBuild::DEVICE, CJNIBuild::BOARD, CJNIBuild::MANUFACTURER,
      CJNIBuild::BRAND, CJNIBuild::MODEL, CJNIBuild::HARDWARE);

  std::string extstorage;
  bool extready = CXBMCApp::GetExternalStorage(extstorage);
  CLog::Log(
      LOGINFO, "External storage path = {}; status = {}; Permissions = {}{}", extstorage,
      extready ? "ok" : "nok",
      CJNIContext::checkCallingOrSelfPermission("android.permission.MANAGE_EXTERNAL_STORAGE") ==
              CJNIPackageManager::PERMISSION_GRANTED
          ? "MANAGE_EXTERNAL_STORAGE "
          : "",
      CJNIContext::checkCallingOrSelfPermission("android.permission.WRITE_EXTERNAL_STORAGE") ==
              CJNIPackageManager::PERMISSION_GRANTED
          ? "WRITE_EXTERNAL_STORAGE"
          : "");
}
