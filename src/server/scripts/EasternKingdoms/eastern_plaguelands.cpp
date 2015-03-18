/*
 * Copyright (C) 2010-2012 Project SkyFire <http://www.projectskyfire.org/>
 * Copyright (C) 2010-2012 Oregon <http://www.oregoncore.com/>
 * Copyright (C) 2006-2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Eastern_Plaguelands
SD%Complete: 100
SDComment: Quest support: 5211, 5742. Special vendor Augustus the Touched
SDCategory: Eastern Plaguelands
EndScriptData */

/* ContentData
mobs_ghoul_flayer
npc_augustus_the_touched
npc_darrowshire_spirit
npc_tirion_fordring
npc_eris_havenfire
EndContentData */

#include "ScriptPCH.h"

//id8530 - cannibal ghoul
//id8531 - gibbering ghoul
//id8532 - diseased flayer

struct mobs_ghoul_flayerAI : public ScriptedAI
{
    mobs_ghoul_flayerAI(Creature *c) : ScriptedAI(c) {}

    void Reset() {}

    void EnterCombat(Unit* /*who*/) {}

    void JustDied(Unit* Killer)
    {
        if (Killer->GetTypeId() == TYPEID_PLAYER)
            me->SummonCreature(11064, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 60000);
    }
};

CreatureAI* GetAI_mobs_ghoul_flayer(Creature* creature)
{
    return new mobs_ghoul_flayerAI (creature);
}

/*######
## npc_augustus_the_touched
######*/

bool GossipHello_npc_augustus_the_touched(Player* player, Creature* creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetGUID());

    if (creature->isVendor() && player->GetQuestRewardStatus(6164))
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
    return true;
}

bool GossipSelect_npc_augustus_the_touched(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        player->SEND_VENDORLIST(creature->GetGUID());
    return true;
}

/*######
## npc_darrowshire_spirit
######*/

#define SPELL_SPIRIT_SPAWNIN    17321

struct npc_darrowshire_spiritAI : public ScriptedAI
{
    npc_darrowshire_spiritAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {
        DoCast(me, SPELL_SPIRIT_SPAWNIN);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void EnterCombat(Unit * /*who*/) {}
};
CreatureAI* GetAI_npc_darrowshire_spirit(Creature* creature)
{
    return new npc_darrowshire_spiritAI (creature);
}

bool GossipHello_npc_darrowshire_spirit(Player* player, Creature* creature)
{
    player->SEND_GOSSIP_MENU(3873, creature->GetGUID());
    player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    return true;
}

/*######
## npc_tirion_fordring
######*/

#define GOSSIP_HELLO    "I am ready to hear your tale, Tirion."
#define GOSSIP_SELECT1  "Thank you, Tirion.  What of your identity?"
#define GOSSIP_SELECT2  "That is terrible."
#define GOSSIP_SELECT3  "I will, Tirion."

bool GossipHello_npc_tirion_fordring(Player* player, Creature* creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetGUID());

    if (player->GetQuestStatus(5742) == QUEST_STATUS_INCOMPLETE && player->getStandState() == UNIT_STAND_STATE_SIT)
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

    return true;
}

bool GossipSelect_npc_tirion_fordring(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(4493, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->SEND_GOSSIP_MENU(4494, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            player->SEND_GOSSIP_MENU(4495, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            player->CLOSE_GOSSIP_MENU();
            player->AreaExploredOrEventHappens(5742);
            break;
    }
    return true;
}

/*######
## npc_eris_havenfire
## This contains script for the quest "The Balance of Light and Shadow"
## Enemy NPCs: Scourge Footsoldier (14486), Cursed Mage (8524), Scourge Archer (14489)
## Friendly NPCs: Injured Peasant (14484), Plagued Peasant (14485)
## Unknown: Priest Epic Event Caller (14493), Invisible Trigger One (14495)
## Spells: Death's Door (23127) - on Injured, Seething Plague (23072)
##
######*/

#define GOSSIP_SELECT1 "The Eye led me here, Eris."
#define GOSSIP_SELECT2 "And I was suited to see this?"
#define GOSSIP_SELECT3 "I really did not know what to expect, Eris. I will use my powers to assist you if that is what is asked of me."
#define GOSSIP_SELECT4 "Those days are over, Eris."
#define GOSSIP_SELECT5 "So what can I do to help?"

enum PriestEpicQuest
{
    NPC_SCOURGE_FOOTSOLDIER = 14486,
    NPC_SCOURGE_ARCHER = 14489,

    NPC_PRIEST_EPIC_EVENT_CALLER = 14493,

    MAX_PEASANT_COUNT = 12,
    NPC_INJURED_PEASANT = 14484,
    NPC_PLAGUED_PEASANT = 14485,

    SPELL_DEATHS_DOOR = 23127,
    SPELL_SEETHING_PLAGUE = 23072,

    QUEST_THE_BALANCE_OF_LIGHT_AND_SHADOW = 7622,

};

static float SpawnPositions[12][4]
{
    // 
    { 3349.141846f, -3050.000488f, 164.789917f, 1.603000f },
    { 3351.238037f, -3051.492432f, 165.248230f, 1.603000f },
    { 3355.165039f, -3047.774902f, 164.961517f, 1.775788f },
    { 3356.926758f, -3050.261963f, 165.239868f, 1.775788f },
    { 3358.251953f, -3052.211426f, 165.419510f, 1.775788f },
    { 3358.324219f, -3045.890869f, 165.264175f, 1.822912f },
    { 3361.271240f, -3046.981934f, 165.156036f, 1.897524f },
    { 3359.340576f, -3042.071289f, 164.130081f, 1.893597f },
    { 3365.473877f, -3046.838135f, 165.095566f, 1.885743f },
    { 3364.936768f, -3049.298096f, 165.206284f, 1.885743f },
    { 3361.816895f, -3046.340088f, 165.083008f, 1.885743f },
};

struct npc_priest_epic_quest_callerAI : public NullCreatureAI
{
    npc_priest_epic_quest_callerAI(Creature *c) : NullCreatureAI(c) {}

    void StartEvent()
    {
        for (uint8 i = 0; i < MAX_PEASANT_COUNT; ++i)
            DoSummon(NPC_INJURED_PEASANT, me, 7.5f, 5000, TEMPSUMMON_CORPSE_TIMED_DESPAWN);
    }
};

CreatureAI* GetAI_npc_priest_epic_quest_caller(Creature* creature)
{
    return new npc_priest_epic_quest_callerAI(creature);
}

bool GossipHello_npc_eris_havenfire(Player* player, Creature* creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetGUID());

    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->SEND_GOSSIP_MENU(7100, creature->GetGUID());

    return true;
}

bool GossipSelect_npc_eris_havenfire(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(7101, creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(7102, creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->SEND_GOSSIP_MENU(7103, creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->SEND_GOSSIP_MENU(7104, creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:
        player->SEND_GOSSIP_MENU(7105, creature->GetGUID());
        break;
    }
    return true;
}

bool QuestAccept_npc_eris_havenfire(Player* /*player*/, Creature* creature, Quest const *quest)
{
    if (quest->GetQuestId() == QUEST_THE_BALANCE_OF_LIGHT_AND_SHADOW)
    {
        if (Creature* pTrigger = creature->FindNearestCreature(NPC_PRIEST_EPIC_EVENT_CALLER, 100.0f))
            CAST_AI(npc_priest_epic_quest_callerAI, pTrigger->AI())->StartEvent();
    }

    return true;
}

void AddSC_eastern_plaguelands()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mobs_ghoul_flayer";
    newscript->GetAI = &GetAI_mobs_ghoul_flayer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_augustus_the_touched";
    newscript->pGossipHello = &GossipHello_npc_augustus_the_touched;
    newscript->pGossipSelect = &GossipSelect_npc_augustus_the_touched;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_darrowshire_spirit";
    newscript->GetAI = &GetAI_npc_darrowshire_spirit;
    newscript->pGossipHello = &GossipHello_npc_darrowshire_spirit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tirion_fordring";
    newscript->pGossipHello =  &GossipHello_npc_tirion_fordring;
    newscript->pGossipSelect = &GossipSelect_npc_tirion_fordring;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_eris_havenfire";
    newscript->pGossipHello = &GossipHello_npc_eris_havenfire;
    newscript->pGossipSelect = &GossipSelect_npc_eris_havenfire;
    newscript->pQuestAccept = &QuestAccept_npc_eris_havenfire;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_priest_epic_quest_caller";
    newscript->GetAI = &GetAI_npc_priest_epic_quest_caller;
    newscript->RegisterSelf();
}

