//     ____                _       __               
//    / __ )____  _____   | |     / /___ ___________
//   / __  / __ \/ ___/   | | /| / / __ `/ ___/ ___/
//  / /_/ / /_/ (__  )    | |/ |/ / /_/ / /  (__  ) 
// /_____/\____/____/     |__/|__/\__,_/_/  /____/  
//                                              
//       A futuristic real-time strategy game.
//          This file is part of Bos Wars.
//
/**@name particlemanager.cpp - The particle manager. */
//
//      (c) Copyright 2007 by Jimmy Salmon
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; only version 2 of the License.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//      02111-1307, USA.
//
//      $Id$

//@{

#include "stratagus.h"
#include "particle.h"
#include "video.h"
#include "ui.h"


CParticleManager ParticleManager;


CParticleManager::CParticleManager() :
	vp(NULL)
{
}

CParticleManager::~CParticleManager()
{
}

void CParticleManager::init()
{
	CExplosion::init();
	CFlameParticle::init();
	CFlashParticle::init();
	CChunkParticle::init();
	CSmokeParticle::init();
}

void CParticleManager::exit()
{
	ParticleManager.clear();

	CExplosion::exit();
	CFlameParticle::exit();
	CFlashParticle::exit();
	CChunkParticle::exit();
	CSmokeParticle::exit();
}

void CParticleManager::clear()
{
	std::vector<CParticle *>::iterator i;
	for (i = particles.begin(); i != particles.end(); ++i) {
		delete *i;
	}
	particles.clear();

	for (i = new_particles.begin(); i != new_particles.end(); ++i) {
		delete *i;
	}
	new_particles.clear();
}

void CParticleManager::draw(const CViewport *vp)
{
	this->vp = vp;

	std::vector<CParticle *>::iterator i;
	for (i = particles.begin(); i != particles.end(); ++i) {
		(*i)->draw();
	}

	this->vp = NULL;
}

void CParticleManager::update()
{
	static unsigned long lastTicks = 0;
	unsigned long ticks = GetTicks() - lastTicks;
	std::vector<CParticle *>::iterator i;

	particles.insert(particles.end(), new_particles.begin(), new_particles.end());
	new_particles.clear();

	i = particles.begin();
	while (i != particles.end()) {
		(*i)->update(ticks);
		if ((*i)->isDestroyed()) {
			delete *i;
			i = particles.erase(i);
		} else {
			++i;
		}
	}

	lastTicks += ticks;
}

void CParticleManager::add(CParticle *particle)
{
	new_particles.push_back(particle);
}

void CParticleManager::add(int type, CPosition &pos)
{
	switch (type) {
		case PARTICLE_EXPLOSION:
			add(new CExplosion(pos));
			break;
		default:
			DebugPrint("Unknown ParticleType: %d\n" _C_ type);
			break;
	}
}

CPosition CParticleManager::getScreenPos(const CPosition &pos)
{
	int x = (int)pos.x;
	int y = (int)pos.y;
	vp->MapPixel2Viewport(x, y);
	return CPosition(x, y);
}

int CParticleManager::getType(const std::string &name)
{
	if (name == "explosion") {
		return PARTICLE_EXPLOSION;
	}

	DebugPrint("Unknown type: %s\n" _C_ name.c_str());
	return PARTICLE_NONE;
}

//@}
