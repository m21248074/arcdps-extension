#include "CombatEventHandler.h"

#include <string>

void CombatEventHandler::Event(cbtevent* pEvent, ag* pSrc, ag* pDst, const char* pSkillname, uint64_t pId, uint64_t pRevision) {
	mSequencer.ProcessEvent(pEvent, pSrc, pDst, pSkillname, pId, pRevision);
}

void CombatEventHandler::EventInternal(cbtevent* pEvent, ag* pSrc, ag* pDst, const char* pSkillname, uint64_t pId) {
    Log(std::format("pId: {}", pId));
    if (pEvent) {
        double time = static_cast<double>(pEvent->time) / 1000;

        if (pEvent->is_statechange) {
            switch (pEvent->is_statechange) {
                case CBTS_ENTERCOMBAT:
                    EnterCombat(time, pEvent->src_agent, static_cast<uint8_t>(pEvent->dst_agent), *pSrc);
                    break;
                case CBTS_EXITCOMBAT:
                    ExitCombat(time, pEvent->src_agent, *pSrc);
                    break;
                case CBTS_CHANGEUP:
                    ChangeUp(time, pEvent->src_agent, *pSrc);
                    break;
                case CBTS_CHANGEDEAD:
                    ChangeDead(time, pEvent->src_agent, *pSrc);
                    break;
                case CBTS_CHANGEDOWN:
                    ChangeDown(time, pEvent->src_agent, *pSrc);
                    break;
//                case CBTS_SPAWN: // Not in realtime api
//                case CBTS_DESPAWN: // Not in realtime api
//                case CBTS_HEALTHUPDATE: // Not in realtime api
                case CBTS_LOGSTART:
                    LogStart(time, pEvent->value, pEvent->buff_dmg, pEvent->src_agent);
                    break;
                case CBTS_LOGEND:
                    LogEnd(time, pEvent->value, pEvent->buff_dmg, pEvent->src_agent);
                    break;
                case CBTS_WEAPSWAP:
                    WeaponSwap(time, pEvent->src_agent, static_cast<WeaponSet>(pEvent->dst_agent), *pSrc);
                    break;
//                case CBTS_MAXHEALTHUPDATE: // Not in realtime api
//                case CBTS_POINTOFVIEW: // Not in realtime api
//                case CBTS_LANGUAGE: // Not in realtime api // Use unofficial extras to get updates of the languge
//                case CBTS_GWBUILD: // Not in realtime api
//                case CBTS_SHARDID: // Not in realtime api
                case CBTS_REWARD:
                    Reward(time, pEvent->src_agent, pEvent->dst_agent, pEvent->value);
                    break;
                case CBTS_BUFFINITIAL: // Not relevant in live api (gives all current boons on LogStart)
                    BuffInitial(time, pEvent, *pSrc, *pDst, pSkillname, pId);
					break;
//                case CBTS_POSITION: // Not in realtime api
//                case CBTS_VELOCITY: // Not in realtime api
//                case CBTS_FACING: // Not in realtime api
                case CBTS_TEAMCHANGE:
                    TeamChange(time, pEvent->src_agent, pEvent->dst_agent, *pSrc);
                    break;
//                case CBTS_ATTACKTARGET: // Not in realtime api
//                case CBTS_TARGETABLE: // Not in realtime api
//                case CBTS_MAPID: // Not in realtime api
//                case CBTS_REPLINFO: // Internal only, not used
                case CBTS_STACKACTIVE:
                    StackActive(time, pEvent->src_agent, pEvent->dst_agent, *pSrc);
                    break;
                case CBTS_STACKRESET: {
                    auto pad = reinterpret_cast<uint32_t*>(&pEvent->pad61);
                    StackReset(time, pEvent->src_agent, pEvent->value, *pad, *pSrc);
                    break;
                }
//                case CBTS_GUILD: // not relevant in live api
//                case CBTS_BUFFINFO: // Not in realtime api
//                case CBTS_BUFFFORMULA: // Not in realtime api
//                case CBTS_SKILLINFO: // Not in realtime api
//                case CBTS_SKILLTIMING: // Not in realtime api
//                case CBTS_BREAKBARSTATE: // Not in realtime api
//                case CBTS_BREAKBARPERCENT: // Not in realtime api
//                case CBTS_ERROR: // Not in realtime api
//                case CBTS_TAG: // Not useful information, it is either delayed or not there at all.
//                case CBTS_BARRIERUPDATE: // Not in realtime api
                case CBTS_STATRESET:
                    StatReset(time);
                    break;
                case CBTS_EXTENSION:
                    Extension(time, pEvent, pSrc, pDst, pSkillname, pId);
                    break;
                case CBTS_APIDELAYED:
                    Delayed(time, pEvent, pSrc, pDst, pSkillname, pId);
                    break;
                case CBTS_INSTANCESTART:
                    InstanceStart(time, pEvent->src_agent);
                    break;
                case CBTS_TICKRATE:
                    Tickrate(time, pEvent->src_agent);
                    break;
                case CBTS_LAST90BEFOREDOWN:
                    Last90BeforeDown(time, pEvent->src_agent, pEvent->dst_agent);
                    break;
//                case CBTS_EFFECT: // Not in realtime api
//                case CBTS_IDTOGUID: // Not in realtime api
            }
        } else if (pEvent->is_activation) {
            Activation(time, pEvent, *pSrc, *pDst, pSkillname, pId);
        } else if (pEvent->is_buffremove) {
            BuffRemove(time, pEvent, *pSrc, *pDst, pSkillname, pId);
        } else if (pEvent->buff) {
            if (pEvent->buff_dmg) {
                BuffDamage(time, pEvent, *pSrc, *pDst, pSkillname, pId);
            } else {
                // Buff apply event
                auto pad = reinterpret_cast<uint32_t*>(&pEvent->pad61);
                BuffApply(time, pEvent, *pSrc, *pDst, pSkillname, pId, *pad);
            }
        } else {
            // Strike damage
            Strike(time, pEvent, *pSrc, *pDst, pSkillname, pId);
        }
    }
    /* pEvent is null. pDst will only be valid on tracking add. pSkillname will also be null */
    else {
        /* notify tracking change */
        if (!pSrc->elite) {
            // only run, when names are set and not null
            if (pSrc->name != nullptr && pSrc->name[0] != '\0' && pDst->name != nullptr && pDst->name[0] != '\0') {

                std::string accountname(pDst->name);

                // remove ':' at the beginning of the name.
                if (accountname.at(0) == ':') {
                    accountname.erase(0, 1);
                }

                /* add */
                if (pSrc->prof) {
                    AgentAdded(accountname, pSrc->name, pSrc->id, pDst->id, pDst->prof, pDst->elite, pDst->self,
                               pSrc->team, static_cast<uint8_t>(pDst->team));
                }
                /* remove */
                else {
                    AgentRemoved(accountname, pSrc->name, pSrc->id, pDst->self);
                }
            }
        }
        /* target change */
        else if (pSrc->elite == 1) {
            TargetChange(pSrc->id);
        }
    }
}
