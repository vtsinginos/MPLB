/*
 * fpi_functions.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: jpd38567
 */

#include "fpi_functions.h"


void DefineNone(int iComponent, int spaceDim, FluidParticleModel modelU) {

	std::map<int, Component> components = g_Components();

	int noElem = 1;
	std::vector<std::string> nameOfRealVars{};
	std::vector<int> sizeRealVars{};
	std::vector<int> sizeIntVars{};
	std::vector<std::string> nameOfIntVars{};
	std::vector<bool> flagParameters{false, false, false, false, false, false, false, false};
	std::vector<Real> variables;

	std::shared_ptr<FpiData> model(new FpiData(components.at(iComponent), spaceDim, modelU, noElem,
			nameOfRealVars, sizeRealVars, nameOfIntVars, sizeIntVars, flagParameters,
			variables));

	fpiModels.insert(std::make_pair(iComponent, model));

}
/***************************************************************************
 * Define flags fpr PSM
 * 	 ownedPostVelocity: False
 *    ownedForceModel: True
 *	 ownedPreCollision: False
 *	 ownedCollisionModel: True
 *	 ownedPostStreaming: false
 *	 ownedDragForce : True
 *	 ownedInitialize: true
 * 	 thermalFlag: false
 */

void DefinePsm(int iComponent, int spaceDim, FluidParticleModel modelU,
		std::vector<Real> variables, SizeType timeStep) {

	std::map<int, Component> components = g_Components();
	//Get elem from mapping_particles
	int nElem= particleMappingModels.at(iComponent)->NumberOfElements();

	std::string name{"Fd"};
	name += std::to_string(iComponent);

	std::vector<std::string> nameOfRealVars{name};//TODO append the component
	std::vector<int> sizeRealVars{spaceDim * nElem};
	std::vector<std::string> nameOfIntVars{};
	std::vector<int> sizeIntVars{};
	std::vector<bool> flagParameters{false, true, false, true, false, true, true, false};

	std::shared_ptr<FpiData> model(new FpiData(components.at(iComponent), spaceDim, modelU, nElem,
				nameOfRealVars, sizeRealVars, nameOfIntVars, sizeIntVars, flagParameters,
				variables, timeStep));

	fpiModels.insert(std::make_pair(iComponent, model));
}
/***************************************************************************
 * Define flags fpr PSM
 * 	 ownedPostVelocity: True
 *    ownedForceModel: True
 *	 ownedPreCollision: False
 *	 ownedCollisionModel: True
 *	 ownedPostStreaming: false
 *	 ownedDragForce : True
 *	 ownedInitialize: true
 * 	 thermalFlag: false
 */

void DefinePrati(int iComponent, int spaceDim, FluidParticleModel modelU,
		std::vector<Real> variables, SizeType timeStep) {

	std::map<int, Component> components = g_Components();
	//Get elem from mapping_particles
	int nElem= particleMappingModels.at(iComponent)->NumberOfElements();

	std::string name{"Fd"};
	name += std::to_string(iComponent);

	std::vector<std::string> nameOfRealVars{name};//TODO append the component
	std::vector<int> sizeRealVars{spaceDim * nElem};
	std::vector<std::string> nameOfIntVars{};
	std::vector<int> sizeIntVars{};
	std::vector<bool> flagParameters{true, true, false, true, false, true, true, false};

	std::shared_ptr<FpiData> model(new FpiData(components.at(iComponent), spaceDim, modelU, nElem,
				nameOfRealVars, sizeRealVars, nameOfIntVars, sizeIntVars, flagParameters,
				variables, timeStep));

	fpiModels.insert(std::make_pair(iComponent, model));
}

void DefineInteractionModels(std::vector<FluidParticleModel> particleModels,
						     std::vector<int> compoId, std::vector<Real> variables,
							 SizeType timeStep) {

	int spaceDim = SpaceDim();
	if (particleModels.size() != compoId.size())  {
		ops_printf("Error: Inconsistent size of particleModels and number of components\n");
	}

	for (int isize = 0; isize < particleModels.size(); isize++) {
		switch (particleModels.at(isize)) {
			case noModel:
				DefineNone(compoId.at(isize), spaceDim, noModel);
				break;
			case PSM:
				DefinePsm(compoId.at(isize), spaceDim, PSM, variables, timeStep);
				break;
			case Prati:
				DefinePrati(compoId.at(isize), spaceDim, Prati, variables, timeStep);
				break;
			default:
				DefinePsm(compoId.at(isize), spaceDim, PSM, variables, timeStep);
		}
	}


}

void FSIVelocityFunctions(std::shared_ptr<FpiData>& fpiPtr) {


	switch(fpiPtr->GetModel()) {
		case Prati:
			FsiPostVelocitiesPrati(fpiPtr);
			break;
		default:
			ops_printf("FSI Velocity function is not supported for this model\n");
	}

}

void FsiForceFunction(std::shared_ptr<FpiData>& fpiPtr) {

	switch (fpiPtr->GetModel()) {
		case PSM:
			FsiForcePSM(fpiPtr);
			break;
		case Prati:
			FsiForcePrati(fpiPtr);
			break;
		default:
			ops_printf("FSi force function must not enter here\n");
	}
}

void FsiPreCollisionFunction(std::shared_ptr<FpiData>& fpiPtr) {

	//TODO In case a new model requires to perform actions prior to Collision step
}

void FsiCollisionFunction(std::shared_ptr<FpiData>& fpiPtr) {

	switch (fpiPtr->GetModel()) {
		case PSM:
			FsiCollisionsPSM(fpiPtr);
			break;
		case Prati:
			FsiCollisionsPrati(fpiPtr);
			break;
		default:
			ops_printf("Fsi collision function must not enter here\n");
	}
}

void FsiPostCollisionFunction(std::shared_ptr<FpiData>& fpiPtr) {

	//TODO  New functions can be enter here
}

void FsiCalculateDragForce(std::shared_ptr<FpiData>& fpiPtr) {

	switch (fpiPtr->GetModel()) {
		case PSM:
			CalculateDragPSM(fpiPtr);
			break;
		case Prati:
			CalculateDragPrati(fpiPtr);
			break;
		default:
			ops_printf("Drag calculation function must not enter here\n");
	}
}

void FsiInitializeFunction(std::shared_ptr<FpiData>& fpiPtr) {
	switch (fpiPtr->GetModel()) {
			case PSM:
				FsiInitializePSM(fpiPtr);
				break;
			case Prati:
				FsiInitializePrati(fpiPtr);
				break;
			default:
				ops_printf("Drag calculation function must not enter here\n");
		}
}


void ObtainData(std::shared_ptr<FpiData>& fpiPtr, int* idVel, int* loop, Real& tauCompo,
		CollisionType& collisModel, int& idComponent,int& rhoId,int &thId) {

	Component& compo(fpiPtr->FluidComponent());
	idVel[0]=  compo.uId;
	idVel[1] = compo.vId;
#ifdef OPS_3D
	idVel[2] = compo.wId;
#endif
	tauCompo = compo.tauRef;
	collisModel = compo.collisionType;
	idComponent = compo.id;

	rhoId = compo.macroVars.at(Variable_Rho).id;

	if (fpiPtr->ThermalFlag())
		thId = compo.macroVars.at(Variable_T).id;
	else
		thId = -1;

}
