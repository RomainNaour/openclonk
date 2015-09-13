/**
	SpawnMenu
	Let's the player spawn all available objects in OpenClonk. 

	@author Maikel
*/


/*-- ClonkControl Interface --*/

// Necessary callbacks for the ClonkControl library.
public func IsSpawnMenu() { return true; }
public func Close() { return RemoveObject(); }
public func Show() { this.Visibility = VIS_Owner; return true; }
public func Hide() { this.Visibility = VIS_None; return true; }

// Close the menu when this object is removed.
func Destruction()
{
	CloseSpawnMenu();
}

// Used as a static function called by the ClonkControl library.
func CreateFor(object cursor)
{
	var obj = CreateObject(GUI_SpawnMenu, AbsX(0), AbsY(0), cursor->GetOwner());
	obj.Visibility = VIS_Owner;
	obj->OpenSpawnMenu(cursor);
	cursor->SetMenu(obj);
	return obj;
}


/*-- Menu Handling --*/

// Menu sizes in percent relative to the screen width and height.
static const SPAWNMENU_Width = 50;
static const SPAWNMENU_Height = 72;
// Number of items per row and minimal size.
static const SPAWNMENU_RowItems = 8;
static const SPAWNMENU_MinItemSize = 6;
// Background colors for hovering and bars and description.
static const SPAWNMENU_HoverColor = 0x50ffffff;
static const SPAWNMENU_BarColor = 0x50888888;
static const SPAWNMENU_DescColor = 0xffaa0000;

local menu_target;
local menu_controller;
local menu;
local menu_id;

public func OpenSpawnMenu(object clonk)
{
	// If the menu is already open, don't open another instance.
	if (clonk->GetMenu() && clonk->GetMenu().ID == menu_id)
		return;	
		
	// Create a menu target for visibility.
	menu_target = CreateObject(Dummy, 0, 0, clonk->GetOwner());
	menu_target.Visibility = VIS_Owner;
	menu_controller = clonk;
	
	// Width should be a multiple of 6 and 8 em.
	// TODO: the 2 is added for a small margin.
	var menu_width = 12 * 2; 
	// Height should be a multiple of 3 em + 12 em.
	var menu_height = 12 + 3 * 8;
	
	// Construction menu proplist.
	menu =
	{
		Target = menu_target,
		Style = GUI_Multiple,
		Decoration = GUI_MenuDeco,
		Left = Format("50%%-%dem", menu_width / 2),
		Right = Format("50%%+%dem", menu_width / 2),
		Top = Format("50%%-%dem", menu_height / 2),
		Bottom = Format("50%%+%dem", menu_height / 2),
		BackgroundColor = {Std = 0},
	};
	
	// Upper bar divides categories.
	MakeCategoryBar();
	
	// Middle area shows the objects to spawn.
	MakeObjectGrid();
	
	// Bottom area show info about the object.
	MakeInfoBar();
	
	// Menu ID.
	menu_id = GuiOpen(menu);
	clonk->SetMenu(this);
	
	MenuShowCategory({Symbol = Shovel, Name = "Items"});
	return;
}

// Returns the proplist of the menu category bar.
private func MakeCategoryBar()
{
	// Create the basic bar submenu.
	var bar = 
	{
		Target = menu_target,
		ID = 100,		
		Left = "0%",
		Right = "100%",
		Top = "0%",
		Bottom = "4em",
		BackgroundColor = {Std = 0}
	};
	// Create the categories: max 6.
	var categories = [{Symbol = Shovel, Name = "Items"}, 
	                  {Symbol = Lorry, Name = "Vehicles"}, 
	                  {Symbol = ToolsWorkshop, Name = "Structures"}, 
	                  {Symbol = Fish, Name = "Animals"},
	                  {Symbol = Tree_Coconut, Name = "Vegetation"}
	                 ];
	for (var i = 0; i < GetLength(categories); ++i)
	{
		var cat = categories[i];
		var cat_id = Format("Cat%d", i + 1);
		var cat_menu = 
		{
			Target = menu_target,
			ID = 101 + i,		
			Left = Format("%dem", 4 * i),
			Right = Format("%dem", 4 * (i + 1)),
			Top = "0%",
			Bottom = "4em",
			Symbol = cat.Symbol,
			BackgroundColor = {Std = 0, Hover = SPAWNMENU_HoverColor},
			OnMouseIn = GuiAction_SetTag("Hover"),
			OnMouseOut = GuiAction_SetTag("Std"),
			OnClick = GuiAction_Call(this, "MenuShowCategory", cat)		
		};
		bar[cat_id] = cat_menu;
	}
	// Add the bar to the menu.
	menu.category_bar = bar;
}

private func MakeObjectGrid()
{
	// Create a submenu for the grid description.
	var grid_desc = 
	{
		Target = menu_target,
		ID = 999,		
		Left = "0%",
		Right = "100%",
		Top = "4em",
		Bottom = "5em",
		Text = "",
		Style = GUI_TextHCenter | GUI_TextVCenter,
		BackgroundColor = {Std = SPAWNMENU_BarColor}
	};
	// Add the grid to the menu.
	menu.grid_desc = grid_desc;
	
	// Create the basic grid submenu.
	var grid = 
	{
		Target = menu_target,
		ID = 1000,		
		Left = "0%",
		Right = "100%+0.1em",
		Top = "5em",
		Bottom = "100%-7.01em",
		Style = GUI_GridLayout,
		BackgroundColor = {Std = 0},
		items = []
	};
	// Add the grid to the menu.
	menu.object_grid = grid;
}

private func MakeInfoBar()
{
	// Create the basic info bar submenu.
	var bar = 
	{
		Target = menu_target,
		ID = 10000,		
		Left = "0%",
		Right = "100%",
		Top = "100%-7em",
		Bottom = "100%",
		BackgroundColor = {Std = 0}
	};
	bar.header = 
	{
		Target = menu_target,
		ID = 10001,	
		Left = "0%",
		Right = "100%",
		Top = "0%",
		Bottom = "1em",
		Text = "",
		Style = GUI_TextHCenter | GUI_TextVCenter,
		BackgroundColor = {Std = SPAWNMENU_BarColor}
	};
	bar.icon = 
	{
		Target = menu_target,
		ID = 10002,	
		Left = "0.5em",
		Right = "5.5em",
		Top = "1.5em",
		Bottom = "6.5em",	
	};
	bar.description = 
	{
		Target = menu_target,
		ID = 10003,	
		Left = "6em",
		Right = "100%",
		Top = "1em",
		Bottom = "4em",
		Text = ""	
	};
	bar.usage = 
	{
		Target = menu_target,
		ID = 10004,	
		Left = "6em",
		Right = "100%",
		Top = "4em",
		Bottom = "7em",
		Text = ""	
	};	
	// Add the grid to the menu.
	menu.info_bar = bar;
}

private func MenuShowCategory(proplist category)
{
	// Update the grid description.
	menu.grid_desc.Text = Format("$MsgCategory$", category.Name);
	GuiUpdate(menu.grid_desc, menu_id, menu.grid_desc.ID, menu_target);
	
	// Determine the id's to show based on the category.
	var id_list = GetItemList(category.Name);		
	// Close all old items.
	for (var i = 0; i < GetLength(menu.object_grid.items); ++i)
	{
		var item = menu.object_grid.items[i];
		GuiClose(menu_id, item.ID, menu_target);
		var item_id = Format("Item%d", i + 1);
		menu.object_grid[item_id] = nil;
	}
	menu.object_grid.Items = [];
	// Add new items.
	for (var i = 0; i < GetLength(id_list); ++i)
	{
		var item = id_list[i];
		var item_id = Format("Item%d", i + 1);
		var item_menu = 
		{
			Target = menu_target,
			ID = 1001 + i,
			Right = "3em",
			Bottom = "3em", 
			Symbol = item,
			BackgroundColor = {Std = 0, Hover = SPAWNMENU_HoverColor},
			OnMouseIn = [GuiAction_SetTag("Hover"), GuiAction_Call(this, "MenuShowInformation", item)],
			OnMouseOut = [GuiAction_SetTag("Std"), GuiAction_Call(this, "MenuHideInformation", item)],
			OnClick = GuiAction_Call(this, "MenuSpawnObject", item)
		};
		menu.object_grid[item_id] = item_menu;
		menu.object_grid.items[i] = item_menu;
	}
	// Update the object grid.
	GuiUpdate(menu.object_grid, menu_id, menu.object_grid.ID, menu_target);
	return;
}

public func MenuShowInformation(id obj_id)
{
	// Update the header: use id if Name property can't be found.
	var name = "";
	if (obj_id)	
 		name = Format("%i", obj_id);
	if (obj_id && obj_id.Name)
		name = obj_id.Name;
	menu.info_bar.header.Text = name;
	GuiUpdate(menu.info_bar.header, menu_id, menu.info_bar.header.ID, menu_target);
	
	// Update the icon.
	menu.info_bar.icon.Symbol = obj_id;
	GuiUpdate(menu.info_bar.icon, menu_id, menu.info_bar.icon.ID, menu_target);
		
	// Update description.
	var desc = "";
	if (obj_id && obj_id.Description)
		desc = Format("$MsgDescription$", SPAWNMENU_DescColor, obj_id.Description);
	menu.info_bar.description.Text = desc;
	GuiUpdate(menu.info_bar.description, menu_id, menu.info_bar.description.ID, menu_target);
	
	// Update usage.
	var usage = "";
	if (obj_id && obj_id.UsageHelp)
		usage = Format("$MsgUsage$", SPAWNMENU_DescColor, obj_id.UsageHelp);
	menu.info_bar.usage.Text = usage;
	GuiUpdate(menu.info_bar.usage, menu_id, menu.info_bar.usage.ID, menu_target);
	return;
}

public func MenuHideInformation(id obj_id)
{
	if (obj_id != menu.info_bar.icon.Symbol)
		return;
		
	// Reset information submenu properties.	
	menu.info_bar.header.Text = "";
	menu.info_bar.icon.Symbol = nil;
	menu.info_bar.description.Text = "";
	menu.info_bar.usage.Text = "";
	
	// Update information submenus.
	GuiUpdate(menu.info_bar.header, menu_id, menu.info_bar.header.ID, menu_target);
	GuiUpdate(menu.info_bar.icon, menu_id, menu.info_bar.icon.ID, menu_target);
	GuiUpdate(menu.info_bar.description, menu_id, menu.info_bar.description.ID, menu_target);
	GuiUpdate(menu.info_bar.usage, menu_id, menu.info_bar.usage.ID, menu_target);
}

public func MenuSpawnObject(id obj_id)
{
	return SpawnObject(obj_id);
}

public func CloseSpawnMenu()
{
	GuiClose(menu_id, nil, menu_target);
	menu_id = nil;
	menu_target->RemoveObject();
	menu_target = nil;
	if (menu_controller)
		menu_controller->MenuClosed();
	menu_controller = nil;
	return;
}


/*-- Object Categories --*/

private func GetItemList(string category)
{
	if (category == "Items")
		return GetItems();
	if (category == "Vehicles")
		return GetVehicles();
	if (category == "Structures")
		return GetStructures();
	if (category == "Animals")
		return GetAnimals();
	if (category == "Vegetation")
		return GetVegetation();
	return [];
}

private func GetItems()
{
	var items = [];
	var index = 0;
	var def;
	while (def = GetDefinition(index++))
	{
		if (!(def->GetCategory() & C4D_Object))
			continue;
		if (!def.Collectible && !def->~IsCarryHeavy())
			continue;
		PushBack(items, def);	
	}
	return items;
}

private func GetVehicles()
{
	var vehicles = [];
	var index = 0;
	var def;
	while (def = GetDefinition(index++))
	{
		if (def->GetCategory() & C4D_Vehicle)
			PushBack(vehicles, def);	
	}
	return vehicles;
}

private func GetStructures()
{
	var structures = [];
	var index = 0;
	var def;
	while (def = GetDefinition(index++))
	{
		if (def->GetCategory() & C4D_Structure)
			PushBack(structures, def);	
	}
	return structures;
}

private func GetAnimals()
{
	var animals = [];
	var index = 0;
	var def;
	while (def = GetDefinition(index++))
	{
		if (!(def->GetCategory() & C4D_Living))
			continue;
		PushBack(animals, def);	
	}
	return animals;
}
	
private func GetVegetation()
{
	var vegetation = [];
	var index = 0;
	var def;
	while (def = GetDefinition(index++))
	{
		if (!def->~IsPlant())
			continue;
		PushBack(vegetation, def);	
	}
	return vegetation;
}

/*-- Object Spawning --*/

private func SpawnObject(id obj_id)
{
	// Safety: there should be someone controlling this menu.
	if (!menu_controller)
		return;
		
	// Create object at the feet of the clonk.
	var obj = menu_controller->CreateObjectAbove(obj_id, 0, 9);	
	
	// Clonk tries to collect the item.
	if (obj.Collectible || obj->~IsCarryHeavy())
	{
		// TODO: find out why this does not work if the clonk has no free hands
		// but has free slots, it works for the contents menu.
		if (menu_controller->Collect(obj, true))
			return;	
	}

	return;
}


/*-- Proplist --*/

local Name = "$Name$";
local Description = "$Description$";
