/***************************************************************************
 *   Copyright (C) 2002~2005 by Yuking                                     *
 *   yuking_net@sohu.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/
//#include <stdio.h>
//#include <limits.h>
//#include <ctype.h>
//#include <libintl.h>
//#include <errno.h>
#include "fcitx/instance.h"

#include "fcitx/fcitx.h"
#include "fcitx/module.h"
//#include "fcitx/hook.h"
//#include "fcitx/candidate.h"
//#include "fcitx/keys.h"
//#include <fcitx/context.h>
//#include "fcitx-config/xdg.h"
//#include "fcitx-utils/log.h"

typedef struct _IMPlugin IMPlugin;

typedef struct _ClosedSourceIM {
    char *swizardpath;//SettingWizard 设置向导软件目录
    char *spath;//Setting 设置属性软件目录
} ClosedSourceIM;

struct _IMPlugin {
    FcitxGenericConfig gconfig;
    ClosedSourceIM chineseime;
    ClosedSourceIM iflyime;
    ClosedSourceIM huayupy;
    FcitxInstance* owner;
};

static void* IMPluginCreate(FcitxInstance* instance);
//static boolean IMSelectorPreFilter(void* arg, FcitxKeySym sym,
//                                   unsigned int state,
//                                   INPUT_RETURN_VALUE *retval
//                                   );
//static  void IMSelectorReset(void* arg);
static  void IMPluginReload(void* arg);
//static INPUT_RETURN_VALUE IMSelectorLocalTrigger(void* arg);
//static INPUT_RETURN_VALUE IMSelectorGlobalTrigger(void* arg);
//static INPUT_RETURN_VALUE IMSelectorClearLocal(void* arg);
//static INPUT_RETURN_VALUE IMSelectorSelect(void* arg);
static boolean LoadIMPluginConfig(IMPlugin* implugin);
static void SaveIMPluginConfig(IMPlugin* implugin);
//static void IMSelectorGetCands(IMSelector* imselector);

FCITX_DEFINE_PLUGIN(implugin, module, FcitxModule) = {
    IMPluginCreate,
    NULL,
    NULL,
    NULL,
    IMPluginReload
};

CONFIG_BINDING_BEGIN(IMPlugin)
CONFIG_BINDING_REGISTER("chineseime", "SettingWizard", chineseime.swizardpath)
CONFIG_BINDING_REGISTER("chineseime", "Setting", chineseime.spath)
CONFIG_BINDING_REGISTER("iflyime", "SettingWizard", iflyime.swizardpath)
CONFIG_BINDING_REGISTER("iflyime", "Setting", iflyime.spath)
CONFIG_BINDING_REGISTER("huayupy", "SettingWizard", huayupy.swizardpath)
CONFIG_BINDING_REGISTER("huayupy", "Setting", huayupy.spath)
CONFIG_BINDING_END()

void* IMPluginCreate(FcitxInstance* instance)
{
    IMPlugin* implugin = fcitx_utils_malloc0(sizeof(IMPlugin));
    implugin->owner = instance;
    if (!LoadIMPluginConfig(implugin)) {
        free(implugin);
        return NULL;
    }

    return implugin;
}

//boolean IMSelectorPreFilter(void* arg, FcitxKeySym sym, unsigned int state, INPUT_RETURN_VALUE* retval)
//{
//    IMSelector* imselector = arg;
//    FcitxInstance* instance = imselector->owner;
//    FcitxInputState *input = FcitxInstanceGetInputState(instance);
//    FcitxGlobalConfig* fc = FcitxInstanceGetGlobalConfig(instance);
//    if (!imselector->triggered)
//        return false;
//    FcitxCandidateWordList* candList = FcitxInputStateGetCandidateList(input);
//    if (FcitxHotkeyIsHotKey(sym, state,
//                            FcitxConfigPrevPageKey(instance, fc))) {
//        FcitxCandidateWordGoPrevPage(candList);
//        *retval = IRV_DISPLAY_MESSAGE;
//    } else if (FcitxHotkeyIsHotKey(sym, state,
//                                   FcitxConfigNextPageKey(instance, fc))) {
//        FcitxCandidateWordGoNextPage(candList);
//        *retval = IRV_DISPLAY_MESSAGE;
//    } else if (FcitxHotkeyIsHotKey(sym, state, FCITX_SPACE)) {
//        if (FcitxCandidateWordPageCount(candList) != 0)
//            *retval = FcitxCandidateWordChooseByIndex(candList, 0);
//    } else if (FcitxHotkeyIsHotKeyDigit(sym, state)) {
//        int iKey = FcitxHotkeyCheckChooseKey(sym, state, DIGIT_STR_CHOOSE);
//        if (iKey >= 0)
//            *retval = FcitxCandidateWordChooseByIndex(candList, iKey);
//    } else if (FcitxHotkeyIsHotKey(sym, state, FCITX_ESCAPE)) {
//        *retval = IRV_CLEAN;
//    }
//    if (*retval == IRV_TO_PROCESS)
//        *retval = IRV_DO_NOTHING;
//    return true;
//}

//INPUT_RETURN_VALUE IMSelectorGlobalTrigger(void* arg)
//{
//    IMSelector* imselector = arg;
//    imselector->triggered = true;
//    imselector->global = true;
//    IMSelectorGetCands(imselector);
//    return IRV_DISPLAY_MESSAGE;
//}

//INPUT_RETURN_VALUE IMSelectorLocalTrigger(void* arg)
//{
//    IMSelector* imselector = arg;
//    imselector->triggered = true;
//    imselector->global = false;
//    IMSelectorGetCands(imselector);
//    return IRV_DISPLAY_MESSAGE;
//}

//INPUT_RETURN_VALUE IMSelectorClearLocal(void* arg)
//{
//    IMSelector* imselector = arg;
//    FcitxInstance* instance = imselector->owner;
//    FcitxInstanceSetLocalIMName(instance, FcitxInstanceGetCurrentIC(instance), NULL);
//    return IRV_CLEAN;
//}

//INPUT_RETURN_VALUE IMSelectorSelect(void* arg)
//{
//    SelectorHandle* handle = arg;
//    IMSelector* imselector = handle->imselector;
//    FcitxInstance* instance = imselector->owner;
//    FcitxIM* im = FcitxInstanceGetIMByIndex(instance, handle->idx);
//    if (!im)
//        return IRV_TO_PROCESS;
//    if (handle->global) {
//        FcitxInstanceSwitchIMByIndex(instance, handle->idx);
//    }
//    else {
//        FcitxInstanceSetLocalIMName(instance, FcitxInstanceGetCurrentIC(instance), im->uniqueName);

//        if (FcitxInstanceGetCurrentState(instance) != IS_ACTIVE)
//            FcitxInstanceEnableIM(instance, FcitxInstanceGetCurrentIC(instance), false);
//    }
//    return IRV_CLEAN;
//}

//INPUT_RETURN_VALUE IMSelectorGetCand(void* arg, FcitxCandidateWord* candWord)
//{
//    IMSelector* imselector = arg;
//    FcitxInstance* instance = imselector->owner;

//    if (!candWord->priv) {
//        FcitxInstanceSetLocalIMName(instance, FcitxInstanceGetCurrentIC(instance), NULL);
//        return IRV_CLEAN;
//    }

//    int index = FcitxInstanceGetIMIndexByName(imselector->owner, (char*) candWord->priv);

//    if (index == 0)
//        FcitxInstanceCloseIM(instance, FcitxInstanceGetCurrentIC(instance));
//    else {
//        if (imselector->global)
//            FcitxInstanceSwitchIMByIndex(instance, index);
//        else
//            FcitxInstanceSetLocalIMName(instance, FcitxInstanceGetCurrentIC(instance), (char*) candWord->priv);

//        if (FcitxInstanceGetCurrentState(instance) != IS_ACTIVE)
//            FcitxInstanceEnableIM(instance, FcitxInstanceGetCurrentIC(instance), false);
//    }
//    return IRV_CLEAN;
//}

//void IMSelectorGetCands(IMSelector* imselector)
//{
//    FcitxInstance* instance = imselector->owner;
//    FcitxInputState *input = FcitxInstanceGetInputState(instance);
//    UT_array* imes = FcitxInstanceGetIMEs(instance);
//    FcitxInstanceSendCloseEvent(instance, CET_SwitchIM);
//    FcitxInstanceCleanInputWindow(instance);

//    FcitxCandidateWordList* candList = FcitxInputStateGetCandidateList(input);
//    FcitxCandidateWordSetPageSize(candList, 10);
//    FcitxCandidateWordSetChoose(candList, DIGIT_STR_CHOOSE);
//    FcitxInputStateSetShowCursor(input, false);
//    FcitxCandidateWordSetOverrideDefaultHighlight(candList, false);
//    FcitxCandidateWordSetLayoutHint(candList, CLH_Vertical);

//    FcitxIM* im = FcitxInstanceGetCurrentIM(instance);
//    FcitxInputContext* ic = FcitxInstanceGetCurrentIC(instance);
//    FcitxInputContext2* ic2 = (FcitxInputContext2*) ic;
//    if (!ic)
//        return;

//    FcitxMessages *aux_up = FcitxInputStateGetAuxUp(input);
//    FcitxMessagesAddMessageStringsAtLast(aux_up, MSG_TIPS, imselector->global ?
//                                         _("Select global input method: ") :
//                                         _("Select local input method: "));
//    if (ic2->imname) {
//        int idx = FcitxInstanceGetIMIndexByName(instance, ic2->imname);
//        FcitxIM *im = fcitx_array_eltptr(imes, idx);
//        if (im) {
//            FcitxMessagesAddMessageStringsAtLast(aux_up, MSG_TIPS,
//                _("Current local input method is "), im->strName);
//        }
//    } else {
//        FcitxMessagesAddMessageStringsAtLast(aux_up, MSG_TIPS,
//                                             _("No local input method"));
//    }

//    utarray_foreach(pim, imes, FcitxIM) {
//        FcitxCandidateWord candWord;
//        candWord.callback = IMSelectorGetCand;
//        candWord.owner = imselector;
//        candWord.strExtra = NULL;
//        if (ic2->imname && strcmp(ic2->imname, pim->uniqueName) == 0) {
//            candWord.priv = NULL;
//            candWord.strWord = strdup(_("Clear local input method"));
//        }
//        else {
//            candWord.priv = strdup(pim->uniqueName);
//            candWord.strWord = strdup(pim->strName);
//        }

//        if (im && strcmp(im->uniqueName, pim->uniqueName) == 0) {
//            candWord.wordType = MSG_CANDIATE_CURSOR;
//        } else {
//            candWord.wordType = MSG_OTHER;
//        }

//        FcitxCandidateWordAppend(candList, &candWord);
//    }
//}

//void IMSelectorReset(void* arg)
//{
//    IMSelector* imselector = arg;
//    imselector->triggered = false;
//}


void IMPluginReload(void* arg)
{
    IMPlugin* implugin = arg;
    LoadIMPluginConfig(implugin);
}

CONFIG_DESC_DEFINE(GetIMPluginConfig, "implugin.desc")

boolean LoadIMPluginConfig(IMPlugin* implugin)
{
    FcitxConfigFileDesc* configDesc = GetIMPluginConfig();
    if (configDesc == NULL)
        return false;

    FILE *fp;
    fp = FcitxXDGGetFileUserWithPrefix("conf", "implugin.config", "r", NULL);
    if (!fp) {
        if (errno == ENOENT)
            SaveIMPluginConfig(implugin);
    }

    FcitxConfigFile *cfile = FcitxConfigParseConfigFileFp(fp, configDesc);

    IMPluginConfigBind(implugin, cfile, configDesc);
    FcitxConfigBindSync((FcitxGenericConfig*)implugin);

    if (fp)
        fclose(fp);

    return true;
}

void SaveIMPluginConfig(IMPlugin* implugin)
{
    FcitxConfigFileDesc* configDesc = GetIMPluginConfig();
    FILE *fp = FcitxXDGGetFileUserWithPrefix("conf", "implugin.config", "w", NULL);
    FcitxConfigSaveConfigFileFp(fp, &implugin->gconfig, configDesc);
    if (fp)
        fclose(fp);
}
