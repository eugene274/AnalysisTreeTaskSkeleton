//
// Created by eugene on 20/02/2021.
//

#ifndef ATTASKSKELETON_AT_TASK_ATI2_ATHELPER_HPP_
#define ATTASKSKELETON_AT_TASK_ATI2_ATHELPER_HPP_

#include <AnalysisTree/EventHeader.hpp>
#include <AnalysisTree/Track.hpp>
#include <AnalysisTree/Particle.hpp>
#include <AnalysisTree/Module.hpp>
#include <AnalysisTree/Hit.hpp>

namespace ATI2 {

namespace ATHelper {

template<typename Entity, typename Value>
void SetField(Entity *data_ptr, Int_t field_id, Value value) {
  data_ptr->SetField(value, field_id);
};

template<typename Value>
void SetField(AnalysisTree::EventHeader *event_header, Int_t field_id, Value value) {
  using AnalysisTree::EventHeaderFields::EventHeaderFields;
  if (field_id >= 0) {
    event_header->template SetField(value, field_id);
  } else {
    if (field_id == EventHeaderFields::kVertexX) {
      event_header->SetVertexPosition3({double(value), event_header->GetVertexY(), event_header->GetVertexZ()});
    } else if (field_id == EventHeaderFields::kVertexY) {
      event_header->SetVertexPosition3({event_header->GetVertexX(), double(value), event_header->GetVertexZ()});
    } else if (field_id == EventHeaderFields::kVertexZ) {
      event_header->SetVertexPosition3({event_header->GetVertexX(), event_header->GetVertexY(), double(value)});
    } else {
      throw std::runtime_error("Invalid field id");
    }
  }
}

template<typename Value>
void SetField(AnalysisTree::Track *track, Int_t field_id, Value value) {
  using AnalysisTree::TrackFields::TrackFields;
  if (field_id >= 0) {
    track->template SetField(value, field_id);
  } else {
    switch (field_id) {
      case TrackFields::kPx: track->SetMomentum3({double(value), track->GetPy(), track->GetPz()});
        break;
      case TrackFields::kPy: track->SetMomentum3({track->GetPx(), double(value), track->GetPz()});
        break;
      case TrackFields::kPz: track->SetMomentum3({track->GetPx(), track->GetPy(), double(value)});
        break;
      case TrackFields::kP:
      case TrackFields::kPt:
      case TrackFields::kEta:
      case TrackFields::kPhi: /* transient fields, ignoring */ break;
      default: throw std::runtime_error("Unknown field");
    }

  }

}

template<typename Value>
void SetField(AnalysisTree::Particle *particle, Int_t field_id, Value value) {
  using AnalysisTree::ParticleFields::ParticleFields;

  if (field_id >= 0) {
    particle->template SetField(value, field_id);
  } else {
    switch (field_id) {
      case ParticleFields::kPx: particle->SetMomentum3({double(value), particle->GetPy(), particle->GetPz()});
        break;
      case ParticleFields::kPy: particle->SetMomentum3({particle->GetPx(), double(value), particle->GetPz()});
        break;
      case ParticleFields::kPz: particle->SetMomentum3({particle->GetPx(), particle->GetPy(), double(value)});
        break;
      case ParticleFields::kMass: particle->SetMass(float(value));
        break;
      case ParticleFields::kPid: particle->SetPid(int(value));
        break;
      case ParticleFields::kP:
      case ParticleFields::kPt:
      case ParticleFields::kEta:
      case ParticleFields::kRapidity:
      case ParticleFields::kPhi: /* transient fields, ignoring */ break;
      default: throw std::runtime_error("Unknown field");
    }
  }

}

template<typename Value>
void SetField(AnalysisTree::Hit *hit, Int_t field_id, Value value) {
  using AnalysisTree::HitFields::HitFields;

  if (field_id >= 0) {
    hit->template SetField(value, field_id);
  } else {
    switch (field_id) {
      case HitFields::kX: hit->SetPosition(double(value), hit->GetY(), hit->GetZ());
        break;
      case HitFields::kY: hit->SetPosition(hit->GetX(), double(value), hit->GetZ());
        break;
      case HitFields::kZ: hit->SetPosition(hit->GetX(), hit->GetY(), double(value));
        break;
      case HitFields::kSignal: hit->SetSignal(double(value));
      case HitFields::kPhi: /* transient fields, ignoring */ break;
      default: throw std::runtime_error("Unknown field");
    }
  }
}

template<typename Value>
void SetField(AnalysisTree::Module *module, Int_t field_id, Value value) {
  using AnalysisTree::ModuleFields::ModuleFields;

  if (field_id >= 0) {
    module->template SetField(value, field_id);
  } else {
    switch (field_id) {
      case ModuleFields::kSignal: module->SetSignal(float(value));
        break;
      case ModuleFields::kNumber: module->SetNumber(short(value));
        break;
      default: throw std::runtime_error("Unknown field");
    }
  }
}

} // namespace ATHelper

} // namespace ATI2

#endif //ATTASKSKELETON_AT_TASK_ATI2_ATHELPER_HPP_
