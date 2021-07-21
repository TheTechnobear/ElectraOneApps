#pragma once

#include "Entity.h"
#include "ParamValue.h"
#include "Parameter.h"

#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <set>

class cJSON;

namespace Kontrol {

class Module;

typedef std::unordered_map<unsigned, std::vector<EntityId>> MidiMap;

typedef std::unordered_map<unsigned, std::vector<EntityId>> ModulationMap;

class ModulePresetValue {
public:
    ModulePresetValue(const EntityId &paramId, const ParamValue &v) :
            paramId_(paramId), value_(v) { ; }

    ModulePresetValue(const ModulePresetValue &src) : paramId_(src.paramId_), value_(src.value_) { ; }

    const EntityId &paramId() { return paramId_; }

    ParamValue value() { return value_; }

private:
    EntityId paramId_;
    ParamValue value_;
};


class ModulePreset {
public:
    ModulePreset() { ; }

    ModulePreset(std::string moduleType,
                 const std::vector<ModulePresetValue> &values,
                 const MidiMap &midimap,
                 const ModulationMap &modmap) :
            moduleType_(moduleType),
            values_(values),
            midi_map_(midimap),
            mod_map_(modmap) {
        ;
    }

    ModulePreset(const ModulePreset &src) : moduleType_(src.moduleType_),
                                            midi_map_(src.midi_map_),
                                            mod_map_(src.mod_map_),
                                            values_(src.values_) { ; }

    ModulePreset &operator=(const ModulePreset &src) {
        moduleType_ = src.moduleType_;
        midi_map_ = src.midi_map_;
        mod_map_ = src.mod_map_;
        values_ = src.values_;
        return *this;
    };

    const std::string &moduleType() const { return moduleType_; }

    const MidiMap &midiMap() const { return midi_map_; }

    const ModulationMap &modulationMap() const { return mod_map_; }

    const std::vector<ModulePresetValue> &values() const { return values_; }

private:
    std::string moduleType_;
    MidiMap midi_map_; // key CC id, value = paramId
    ModulationMap mod_map_; // key bus id, value = paramId
    std::vector<ModulePresetValue> values_;
};


class KontrolModel;


class Rack : public Entity {
public:
    Rack(const std::string &host,
         unsigned port,
         const std::string &displayName
         ) :    Entity(createId(host, port), displayName),
                host_(host), port_(port),
                mainDir_("."),
                dataDir_("./data/orac"),
                mediaDir_("./media"),
                userModuleDir_("./usermodules"),
                moduleDir_("modules") {
    }

    // void initPrefs();

    static EntityId createId(const std::string &host, unsigned port) {
        return (host + ":" + std::to_string(port));
    }

    std::vector<std::shared_ptr<Module>> getModules();
    std::shared_ptr<Module> getModule(const EntityId &moduleId);
    void addModule(const std::shared_ptr<Module> &module);


    // bool loadModuleDefinitions(const EntityId &moduleId, const mec::Preferences &prefs);

    // bool loadSettings(const std::string &filename);
    // bool loadSettings(const mec::Preferences &prefs);

    // bool saveSettings();
    // bool saveSettings(const std::string &filename);

    // local racks
    // bool loadPreset(std::string presetId);
    // bool savePreset(std::string presetId);
    std::vector<std::string> getPresetList();

    const std::string &mainDir() const { return mainDir_; }
    const std::string &dataDir() const { return dataDir_; }
    const std::string &mediaDir() const { return mediaDir_; }
    const std::string &moduleDir() const { return moduleDir_; }
    const std::string &userModuleDir() const { return userModuleDir_; }
    void mainDir(const std::string &d) { mainDir_ = d; }
    void dataDir(const std::string &d) { dataDir_ = d; }
    void mediaDir(const std::string &d) { mediaDir_ = d; }
    void moduleDir(const std::string &d) { moduleDir_ = d; }
    void userModuleDir(const std::string &d) { userModuleDir_ = d; }

    // used on non-local rack
    const std::string &currentPreset() const { return currentPreset_; }

    void currentPreset(const std::string &preset) { currentPreset_ = preset; }


    bool changeMidiCC(unsigned midiCC, unsigned midiValue);
    void addMidiCCMapping(unsigned ccnum, const EntityId &moduleId, const EntityId &paramId);
    void removeMidiCCMapping(unsigned ccnum, const EntityId &moduleId, const EntityId &paramId);


    bool changeModulation(unsigned bus, float value);
    void addModulationMapping(const std::string &src, unsigned bus, const EntityId &moduleId, const EntityId &paramId);
    void removeModulationMapping(const std::string &src, unsigned bus, const EntityId &moduleId, const EntityId &paramId);


    static std::shared_ptr<KontrolModel> model();

    void publishMetaData(const std::shared_ptr<Module> &module) const;
    void publishMetaData() const;

    void publishCurrentValues(const std::shared_ptr<Module> &module) const;
    void publishCurrentValues() const;

    std::set<std::string> getResourceTypes();
    const std::set<std::string> &getResources(const std::string &type);
    void addResource(const std::string &type, const std::string &resource);


    void dumpSettings() const;
    void dumpParameters();
    void dumpCurrentValues();

    const std::string &host() const { return host_; }

    unsigned port() const { return port_; }

private:
    typedef std::unordered_map<EntityId, ModulePreset> RackPreset;
    // bool loadFilePreset(const std::string &presetId);
    // bool saveFilePreset(const std::string &presetId);

    // bool loadModulePreset(RackPreset &rackPreset, const EntityId &moduleId, const mec::Preferences &prefs);
    // bool saveModulePreset(ModulePreset &, cJSON *root);
    bool updateModulePreset(std::shared_ptr<Module> module, ModulePreset &modulePreset);
    bool applyModulePreset(std::shared_ptr<Module> module, const ModulePreset &modulePreset);

    // platform prefs
    std::string host_;
    unsigned port_;
    std::string mainDir_;
    std::string mediaDir_;
    std::string dataDir_;
    std::string moduleDir_;
    std::string userModuleDir_;

    // user prefs for rack
    std::string currentPreset_;

    std::string settingsFile_;
    // std::shared_ptr<mec::Preferences> settings_;

    std::map<EntityId, std::shared_ptr<Module>> modules_;
    std::unordered_map<std::string, std::set<std::string>> resources_;
    std::vector<std::string> presets_;
    RackPreset rackPreset_;
};

}