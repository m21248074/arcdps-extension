#pragma once

#include "../MainTable.h"

#include "../../arcdps_structs.h"

static const std::vector<MainTableColumn> tableColumns = {
	{0, []{return "Column1";}, []{ return nullptr; }, 0},
	{1, []{return "Column2";}, []{ return nullptr; }, 0},
	{2, []{return "Column3";}, []{ return nullptr; }, 1},
	{3, []{return "Column4";}, []{ return nullptr; }, 2},
};

/**
 * Hardcoded rows, this is just here for this 
 */
struct Row {
	std::string Column1;
	std::string Column2;
	std::string Column3;
	std::string Column4;
};

static inline std::vector<Row> Rows = {
	{"1Row1.1", "4Row1.2", "3Row1.3", "3Row1.4"},
	{"2Row2.1", "3Row2.2", "5Row2.3", "1Row2.4"},
	{"3Row3.1", "5Row3.2", "2Row3.3", "5Row3.4"},
	{"4Row4.1", "2Row4.2", "1Row4.3", "4Row4.4"},
	{"5Row5.1", "1Row5.2", "4Row5.3", "2Row5.4"},
};

class DemoTable : public MainTable<> {
public:
	DemoTable(MainWindow* pMainWindow) : MainTable<>(tableColumns, pMainWindow, MainTableFlags_SubWindow) {}

protected:
	Alignment& getAlignment() override;
	Alignment& getHeaderAlignment() override;
	std::string getTableId() override;
	int& getMaxDisplayed() override;
	const char* getCategoryName(int pCat) override;
	bool& getShowAlternatingBackground() override;
	TableSettings& getTableSettings() override;

	void DrawRows(ImGuiTableColumnIdx pFirstColumnIndex) override;
	void Sort(const ImGuiTableColumnSortSpecs* mColumnSortSpecs) override;

private:
	Alignment mAlignment = Alignment::Left;
	Alignment mHeaderAlignment = Alignment::Left;
	int mMaxDisplayed = 0;
	bool mAlternatingBackground = false;
	TableSettings mSettings;
};
