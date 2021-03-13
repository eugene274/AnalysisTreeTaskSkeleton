//
// Created by eugene on 31/08/2020.
//

#ifndef ANALYSISTREESKELETON_SAMPLE_TASK_SAMPLETASK_H
#define ANALYSISTREESKELETON_SAMPLE_TASK_SAMPLETASK_H

#include <TChain.h>

#include <at_task/Task.h>
#include <at_task/StaticTaskRegistry.h>

#include <AnalysisTree/Detector.hpp>
#include <AnalysisTree/Module.hpp>
#include <AnalysisTree/EventHeader.hpp>

class FooTask : public UserFillTask {
public:
  boost::program_options::options_description GetBoostOptions() override {
    /* insert your options here if need */
    return boost::program_options::options_description(GetName() + " options");
  }
  void PreInit() override {

  }

  void UserInit(std::map<std::string, void *> & /* map */) override {

  }
  void UserExec() override {

  }
  void UserFinish() override {

  }

private:

TASK_DEF(FooTask, 1)
};

class BarTask : public UserFillTask {

public:
  void PreInit() override {
    auto foo_task = FooTask::Instance();
    std::cout << foo_task->GetName() << std::endl;
  }

  void UserInit(std::map<std::string, void *> & /* map */) override {
    BypassBranches();
    rec_event_header_vtx_x = GetVar("RecEventHeader/vtx_x");
    rec_event_header_vtx_x.Print();

    vtx_tracks_branch = GetInBranch("VtxTracks");

    vtxtracks_dca_x = GetVar("VtxTracks/dcax");
    vtxtracks_dca_x.Print();

    processed_tracks_branch = NewBranch("ProcessedTracks", PARTICLES);
    processed_tracks_branch->CloneVariables(vtx_tracks_branch->GetConfig());
    processed_tracks_branch->UseFields({{"dcax", processed_tracks_dcax}});

    /* We will take dcax/y from VtxTracks */
//    processed_tracks_dcax = processed_tracks_branch->NewVariable("dcax", FLOAT);
//    error: already exists
    processed_tracks_vtxx = processed_tracks_branch->NewVariable("vtx_x", FLOAT);

    NewBranch("test_event_header", EVENT_HEADER);
    test_event_header = GetOutBranch("test_event_header");
    test_event_header->UseFields({
                                     {"vtx_x", test_event_header_vtx_x},
                                     {"vtx_y", test_event_header_vtx_y},
                                     {"vtx_z", test_event_header_vtx_z}
                                 });
    rec_event_header = GetInBranch("RecEventHeader");

    test_event_header->Freeze(); /* No more structural changes */
    processed_tracks_branch->Freeze();

  }
  void UserExec() override {

    /* Abilities of ATI2::Branch */
    test_event_header->CopyContents(rec_event_header);
//    std::cout << (*rec_event_header)[rec_event_header_vtx_x] << "\t"
//              << (*test_event_header)[test_event_header_vtx_x] << std::endl;




    auto vtx_x_val = *rec_event_header_vtx_x;
//    std::cout << float(vtx_x_val) << std::endl;


//    std::cout << dca_x.Get<float>() << std::endl; /* not implemented */
//    GetBranch("RecEventHeader")->size(); /* not implemented */

    processed_tracks_branch->ClearChannels();
    /* abilities of BranchChannel */
    for (auto &vtx_track : vtx_tracks_branch->Loop()) {
      auto processed_track = processed_tracks_branch->NewChannel();
      processed_track.CopyContents(vtx_track);
      processed_track.CopyContents(*rec_event_header);

      processed_track[processed_tracks_dcax] = true;
      processed_track[processed_tracks_dcax] = 0.0;
      processed_track[processed_tracks_dcax] = 1;

//      std::cout << "Current size " << processed_tracks_branch->size() << std::endl;
//      vtxtracks_dca_x.Print();
//      std::cout << vtx_track[vtxtracks_dca_x].GetVal() << "\t"
//                << processed_track[processed_tracks_dcax].GetVal() << std::endl;
//      rec_event_header_vtx_x.Print();
      processed_tracks_vtxx.Print();
      std::cout << (*rec_event_header)[rec_event_header_vtx_x].GetVal() << "\t"
                << processed_track[processed_tracks_vtxx].GetVal() << std::endl;

    }

  }
  void UserFinish() override {

  }

private:

  ATI2::Branch *rec_event_header{nullptr};
  ATI2::Variable rec_event_header_vtx_x;

  ATI2::Branch *test_event_header{nullptr};

  ATI2::Variable test_event_header_vtx_x;
  ATI2::Variable test_event_header_vtx_y;
  ATI2::Variable test_event_header_vtx_z;
  ATI2::Variable vtxtracks_dca_x;
  ATI2::Branch *processed_tracks_branch;
  ATI2::Variable processed_tracks_dcax;
  ATI2::Variable processed_tracks_vtxx;

  ATI2::Branch *vtx_tracks_branch;
 TASK_DEF(BarTask, 1);
};

#endif //ANALYSISTREESKELETON_SAMPLE_TASK_SAMPLETASK_H
