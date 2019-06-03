﻿namespace hfsm2 {
namespace detail {

////////////////////////////////////////////////////////////////////////////////

template <StateID NHeadIndex,
		  ShortIndex NCompoIndex,
		  ShortIndex NOrthoIndex,
		  typename TArgs,
		  RegionStrategy TStrategy,
		  typename THead,
		  typename... TSubStates>
struct _C {
	static constexpr StateID	HEAD_ID		 = NHeadIndex;
	static constexpr ShortIndex COMPO_INDEX	 = NCompoIndex;
	static constexpr ShortIndex ORTHO_INDEX	 = NOrthoIndex;
	static constexpr ShortIndex REGION_ID	 = COMPO_INDEX + ORTHO_INDEX;
	static constexpr ForkID		COMPO_ID	 = COMPO_INDEX + 1;

	static constexpr RegionStrategy STRATEGY = TStrategy;

	using Args			= TArgs;
	using Head			= THead;

	using Utility		= typename Args::Utility;
	using UP			= typename Args::UP;
	using StateList		= typename Args::StateList;
	using RegionList	= typename Args::RegionList;
	using Payload		= typename Args::Payload;

	using Request		= RequestT<Payload>;
	using RequestType	= typename Request::Type;

	using StateRegistry	= StateRegistryT<Args>;
	using StateParents	= typename StateRegistry::StateParents;
	using RemainBit		= typename StateRegistry::CompoRemains::Bit;

	using Control		= ControlT<Args>;

	using PlanControl	= PlanControlT<Args>;
	using ScopedOrigin	= typename PlanControl::Origin;
	using ScopedRegion	= typename PlanControl::Region;
	using Plan			= PlanT<Args>;

	using FullControl	= FullControlT<Args>;
	using ControlLock	= typename FullControl::Lock;

	using GuardControl	= GuardControlT<Args>;


	using HeadState		= _S <HEAD_ID, Args, Head>;
	using SubStates		= _CS<HEAD_ID + 1, COMPO_INDEX + 1, ORTHO_INDEX, Args, STRATEGY, 0, TSubStates...>;
	using AllForward	= _CF<STRATEGY, Head, TSubStates...>;

	static constexpr ShortIndex REGION_SIZE	= AllForward::STATE_COUNT;

	HFSM_INLINE ShortIndex& compoActive   (StateRegistry& stateRegistry)	{ return stateRegistry.compoActive	  [COMPO_INDEX]; }
	HFSM_INLINE ShortIndex& compoResumable(StateRegistry& stateRegistry)	{ return stateRegistry.resumable.compo[COMPO_INDEX]; }
	HFSM_INLINE ShortIndex& compoRequested(StateRegistry& stateRegistry)	{ return stateRegistry.requested.compo[COMPO_INDEX]; }

	HFSM_INLINE ShortIndex& compoActive   (Control& control)				{ return compoActive   (control._stateRegistry); }
	HFSM_INLINE ShortIndex& compoResumable(Control& control)				{ return compoResumable(control._stateRegistry); }
	HFSM_INLINE ShortIndex& compoRequested(Control& control)				{ return compoRequested(control._stateRegistry); }

	HFSM_INLINE RemainBit	compoRemain	  (Control& control)				{ return control._stateRegistry.compoRemains   [COMPO_INDEX]; }

	HFSM_INLINE void	deepRegister				  (StateRegistry& stateRegistry, const Parent parent);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	HFSM_INLINE bool	deepForwardEntryGuard		  (GuardControl& control,						const ShortIndex = INVALID_SHORT_INDEX);
	HFSM_INLINE bool	deepEntryGuard				  (GuardControl& control,						const ShortIndex = INVALID_SHORT_INDEX);

	HFSM_INLINE void	deepEnter					  (PlanControl&  control,						const ShortIndex = INVALID_SHORT_INDEX);
	HFSM_INLINE void	deepReenter					  (PlanControl&  control,						const ShortIndex = INVALID_SHORT_INDEX);

	HFSM_INLINE Status	deepUpdate					  (FullControl&  control,						const ShortIndex = INVALID_SHORT_INDEX);

	template <typename TEvent>
	HFSM_INLINE Status	deepReact					  (FullControl&  control, const TEvent& event,	const ShortIndex = INVALID_SHORT_INDEX);

	HFSM_INLINE bool	deepForwardExitGuard		  (GuardControl& control,						const ShortIndex = INVALID_SHORT_INDEX);
	HFSM_INLINE bool	deepExitGuard				  (GuardControl& control,						const ShortIndex = INVALID_SHORT_INDEX);

	HFSM_INLINE void	deepExit					  (PlanControl&  control,						const ShortIndex = INVALID_SHORT_INDEX);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	HFSM_INLINE void   deepForwardActive			  (Control& control, const RequestType request,	const ShortIndex = INVALID_SHORT_INDEX);
	HFSM_INLINE void   deepForwardRequest			  (Control& control, const RequestType request,	const ShortIndex = INVALID_SHORT_INDEX);

	HFSM_INLINE void   deepRequest					  (Control& control, const RequestType request);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if HFSM_EXPLICIT_MEMBER_SPECIALIZATION

	template <RegionStrategy TG = STRATEGY>
	HFSM_INLINE void	deepRequestChange			  (Control& control, const ShortIndex = INVALID_SHORT_INDEX);

	template <>
	HFSM_INLINE void	deepRequestChange<Composite>  (Control& control, const ShortIndex)	{ deepRequestChangeComposite  (control); }

	template <>
	HFSM_INLINE void	deepRequestChange<Resumable>  (Control& control, const ShortIndex)	{ deepRequestChangeResumable  (control); }

	template <>
	HFSM_INLINE void	deepRequestChange<Utilitarian>(Control& control, const ShortIndex)	{ deepRequestChangeUtilitarian(control); }

#else

	HFSM_INLINE void	deepRequestChange			  (Control& control, const ShortIndex = INVALID_SHORT_INDEX);

#endif

	HFSM_INLINE void	deepRequestChangeComposite	  (Control& control);
	HFSM_INLINE void	deepRequestChangeResumable	  (Control& control);
	HFSM_INLINE void	deepRequestChangeUtilitarian  (Control& control);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	HFSM_INLINE void	deepRequestRemain			  (StateRegistry& stateRegistry);
	HFSM_INLINE void	deepRequestRestart			  (StateRegistry& stateRegistry);
	HFSM_INLINE void	deepRequestResume			  (StateRegistry& stateRegistry,				const ShortIndex = INVALID_SHORT_INDEX);
	HFSM_INLINE void	deepRequestUtilize			  (Control& control);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if HFSM_EXPLICIT_MEMBER_SPECIALIZATION

	template <RegionStrategy TG = STRATEGY>
	HFSM_INLINE UP		deepReportChange			  (Control& control, const ShortIndex = INVALID_SHORT_INDEX);

	template <>
	HFSM_INLINE UP		deepReportChange<Composite>   (Control& control, const ShortIndex)	{ return deepReportChangeComposite  (control); }

	template <>
	HFSM_INLINE UP		deepReportChange<Resumable>   (Control& control, const ShortIndex)	{ return deepReportChangeResumable  (control); }

	template <>
	HFSM_INLINE UP		deepReportChange<Utilitarian> (Control& control, const ShortIndex)	{ return deepReportChangeUtilitarian(control); }

#else

	HFSM_INLINE UP		deepReportChange			  (Control& control, const ShortIndex = INVALID_SHORT_INDEX);

#endif

	HFSM_INLINE UP		deepReportChangeComposite	  (Control& control);
	HFSM_INLINE UP		deepReportChangeResumable	  (Control& control);
	HFSM_INLINE UP		deepReportChangeUtilitarian   (Control& control);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// TODO: add 'deepReportRemain/Restart/Resume()'
	HFSM_INLINE UP		deepReportUtilize			  (Control& control);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	HFSM_INLINE void	deepEnterRequested			  (PlanControl& control,						const ShortIndex = INVALID_SHORT_INDEX);
	HFSM_INLINE void	deepChangeToRequested		  (PlanControl& control,						const ShortIndex = INVALID_SHORT_INDEX);

#ifdef HFSM_ENABLE_STRUCTURE_REPORT
	using RegionType	= typename StructureStateInfo::RegionType;

	static constexpr LongIndex NAME_COUNT = HeadState::NAME_COUNT + SubStates::NAME_COUNT;

	void deepGetNames(const LongIndex parent,
					  const RegionType region,
					  const ShortIndex depth,
					  StructureStateInfos& stateInfos) const;
#endif

	HeadState _headState;
	SubStates _subStates;
};

////////////////////////////////////////////////////////////////////////////////

}
}

#include "composite.inl"