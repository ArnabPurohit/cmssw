#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"
#include "DataFormats/ForwardDetId/interface/HGCTriggerDetId.h"

#include <vector>
#include <iostream>
#include <fstream>


class HGCalTriggerGeometryHexImp1 : public HGCalTriggerGeometryBase
{
    public:
        HGCalTriggerGeometryHexImp1(const edm::ParameterSet& conf);

        virtual void initialize(const es_info& ) override final;

    private:
        edm::FileInPath l1tCellsMapping_;
        edm::FileInPath l1tModulesMapping_;

        void fillMaps(const es_info&);
        void buildTriggerCellsAndModules(const es_info&);
};


/*****************************************************************/
HGCalTriggerGeometryHexImp1::HGCalTriggerGeometryHexImp1(const edm::ParameterSet& conf):
    HGCalTriggerGeometryBase(conf),
    l1tCellsMapping_(conf.getParameter<edm::FileInPath>("L1TCellsMapping")),
    l1tModulesMapping_(conf.getParameter<edm::FileInPath>("L1TModulesMapping"))
/*****************************************************************/
{
}


/*****************************************************************/
void HGCalTriggerGeometryHexImp1::initialize(const es_info& esInfo)
/*****************************************************************/
{
    edm::LogWarning("HGCalTriggerGeometry") << "WARNING: This HGCal trigger geometry is incomplete.\n"\
                                            << "WARNING: There is no neighbor information.\n";

    fillMaps(esInfo);
    buildTriggerCellsAndModules(esInfo);

}



/*****************************************************************/
void HGCalTriggerGeometryHexImp1::fillMaps(const es_info& esInfo)
/*****************************************************************/
{
    //
    // read module mapping file
    std::map<short, short> wafer_to_module_ee;
    std::map<short, short> wafer_to_module_fh;
    std::ifstream l1tModulesMappingStream(l1tModulesMapping_.fullPath());
    if(!l1tModulesMappingStream.is_open()) edm::LogError("HGCalTriggerGeometry") << "Cannot open L1TModulesMapping file\n";
    short subdet  = 0;
    short wafer   = 0;
    short module  = 0;
    for(; l1tModulesMappingStream>>subdet>>wafer>>module; )
    {
        if(subdet==3) wafer_to_module_ee.emplace(wafer,module);
        else if(subdet==4) wafer_to_module_fh.emplace(wafer,module);
        else edm::LogWarning("HGCalTriggerGeometry") << "Unsupported subdetector number ("<<subdet<<") in L1TModulesMapping file\n";
    }
    if(!l1tModulesMappingStream.eof()) edm::LogWarning("HGCalTriggerGeometry") << "Error reading L1TModulesMapping '"<<wafer<<" "<<module<<"' \n";
    l1tModulesMappingStream.close();
    // read trigger cell mapping file
    std::map<std::pair<short,short>, short> cells_to_trigger_cells;
    std::ifstream l1tCellsMappingStream(l1tCellsMapping_.fullPath());
    if(!l1tCellsMappingStream.is_open()) edm::LogError("HGCalTriggerGeometry") << "Cannot open L1TCellsMapping file\n";
    short waferType   = 0;
    short cell        = 0;
    short triggerCell = 0;
    for(; l1tCellsMappingStream>>waferType>>cell>>triggerCell; )
    {
        cells_to_trigger_cells.emplace(std::make_pair((waferType?1:-1),cell), triggerCell);
    }
    if(!l1tCellsMappingStream.eof()) edm::LogWarning("HGCalTriggerGeometry") << "Error reading L1TCellsMapping'"<<waferType<<" "<<cell<<" "<<triggerCell<<"' \n";
    l1tCellsMappingStream.close();
    //
    // Loop over HGC cells
    // EE
    for(const auto& id : esInfo.geom_ee->getValidGeomDetIds())
    {
        if(id.rawId()==0) continue;
        HGCalDetId waferDetId(id); 
        short module      = wafer_to_module_ee[waferDetId.wafer()];
        int nCells = esInfo.topo_ee->dddConstants().numberCellsHexagon(waferDetId.wafer());
        for(int c=0;c<nCells;c++)
        {
            short triggerCellId = cells_to_trigger_cells[std::make_pair(waferDetId.waferType(),c)];
            // Fill cell -> trigger cell mapping
            HGCalDetId cellDetId(ForwardSubdetector(waferDetId.subdetId()), waferDetId.zside(), waferDetId.layer(), waferDetId.waferType(), waferDetId.wafer(), c); 
            HGCalDetId triggerCellDetId(ForwardSubdetector(waferDetId.subdetId()), waferDetId.zside(), waferDetId.layer(), waferDetId.waferType(), waferDetId.wafer(), triggerCellId); 
            cells_to_trigger_cells_.emplace(cellDetId, triggerCellDetId);
            // Fill trigger cell -> module mapping
            HGCalDetId moduleDetId(ForwardSubdetector(waferDetId.subdetId()), waferDetId.zside(), waferDetId.layer(), waferDetId.waferType(), module, HGCalDetId::kHGCalCellMask);
            trigger_cells_to_modules_.emplace(triggerCellDetId, moduleDetId); 
        }
    }
    // FH
    for(const auto& id : esInfo.geom_fh->getValidGeomDetIds())
    {
        if(id.rawId()==0) continue;
        HGCalDetId waferDetId(id); 
        short module      = wafer_to_module_fh[waferDetId.wafer()];
        int nCells = esInfo.topo_ee->dddConstants().numberCellsHexagon(waferDetId.wafer());
        for(int c=0;c<nCells;c++)
        {
            short triggerCellId = cells_to_trigger_cells[std::make_pair(waferDetId.waferType(),c)];
            // Fill cell -> trigger cell mapping
            HGCalDetId cellDetId(ForwardSubdetector(waferDetId.subdetId()), waferDetId.zside(), waferDetId.layer(), waferDetId.waferType(), waferDetId.wafer(), c); 
            HGCalDetId triggerCellDetId(ForwardSubdetector(waferDetId.subdetId()), waferDetId.zside(), waferDetId.layer(), waferDetId.waferType(), waferDetId.wafer(), triggerCellId); 
            cells_to_trigger_cells_.emplace(cellDetId, triggerCellDetId);
            // Fill trigger cell -> module mapping
            HGCalDetId moduleDetId(ForwardSubdetector(waferDetId.subdetId()), waferDetId.zside(), waferDetId.layer(), waferDetId.waferType(), module, HGCalDetId::kHGCalCellMask);
            trigger_cells_to_modules_.emplace(triggerCellDetId, moduleDetId); 
        }
    }
}



/*****************************************************************/
void HGCalTriggerGeometryHexImp1::buildTriggerCellsAndModules(const es_info& esInfo)
/*****************************************************************/
{
    //
    // Build trigger cells and fill map
    typedef HGCalTriggerGeometry::TriggerCell::list_type list_cells;
    // make list of cells in trigger cells
    std::map<unsigned, list_cells> trigger_cells_to_cells;
    for(const auto& cell_triggerCell : cells_to_trigger_cells_)
    {
        unsigned cell        = cell_triggerCell.first;
        unsigned triggerCell = cell_triggerCell.second;
        auto itr_exist       = trigger_cells_to_cells.emplace(triggerCell, list_cells());
        itr_exist.first->second.emplace(cell);
        //trigger_cells_to_cells.at(triggerCell).emplace(cell);
    }
    for(const auto& triggerCell_cells : trigger_cells_to_cells)
    {
        unsigned triggerCellId = triggerCell_cells.first;
        list_cells cellIds = triggerCell_cells.second;
        // Position: barycenter of the trigger cell.
        Basic3DVector<float> triggerCellVector(0.,0.,0.);
        for(const auto& cell : cellIds)
        {
            HGCalDetId cellDetId(cell);
            triggerCellVector += esInfo.geom_ee->getPosition(cellDetId).basicVector();
        }
        GlobalPoint triggerCellPoint( triggerCellVector/cellIds.size() );
        const auto& triggerCellToModuleItr = trigger_cells_to_modules_.find(triggerCellId);
        unsigned moduleId = (triggerCellToModuleItr!=trigger_cells_to_modules_.end() ? triggerCellToModuleItr->second : 0); // 0 if the trigger cell doesn't belong to a module
        //unsigned moduleId = trigger_cells_to_modules_.at(triggercellId);
        // FIXME: empty neighbours
        trigger_cells_.emplace(triggerCellId, std::make_unique<const HGCalTriggerGeometry::TriggerCell>(triggerCellId, moduleId, triggerCellPoint, list_cells(), cellIds));
    }
    //
    // Build modules and fill map
    typedef HGCalTriggerGeometry::Module::list_type list_triggerCells;
    typedef HGCalTriggerGeometry::Module::tc_map_type tc_map_to_cells;
    // make list of trigger cells in modules
    std::map<unsigned, list_triggerCells> modules_to_trigger_cells;
    for(const auto& triggerCell_module : trigger_cells_to_modules_)
    {
        unsigned triggerCell = triggerCell_module.first;
        unsigned module      = triggerCell_module.second;
        auto itr_exist       = modules_to_trigger_cells.emplace(module, list_triggerCells());
        itr_exist.first->second.emplace(triggerCell);
        //modules_to_trigger_cells.at(module).emplace(triggerCell);
    }
    for(const auto& module_triggerCell : modules_to_trigger_cells)
    {
        unsigned moduleId = module_triggerCell.first;
        list_triggerCells triggerCellIds = module_triggerCell.second;
        tc_map_to_cells cellsInTriggerCells;
        // Position: barycenter of the module, from trigger cell positions
        Basic3DVector<float> moduleVector(0.,0.,0.);
        for(const auto& triggerCell : triggerCellIds)
        {
            const auto& cells_in_tc = trigger_cells_to_cells.at(triggerCell);
            for( const unsigned cell : cells_in_tc ) 
            {
              cellsInTriggerCells.emplace(triggerCell,cell);
            }
            moduleVector += trigger_cells_.at(triggerCell)->position().basicVector();
        }
        GlobalPoint modulePoint( moduleVector/triggerCellIds.size() );
        // FIXME: empty neighbours
        modules_.emplace(moduleId, std::make_unique<const HGCalTriggerGeometry::Module>(moduleId, modulePoint, list_triggerCells(), triggerCellIds, cellsInTriggerCells));
    }
}


DEFINE_EDM_PLUGIN(HGCalTriggerGeometryFactory, 
        HGCalTriggerGeometryHexImp1,
        "HGCalTriggerGeometryHexImp1");
