/*
 * OpenClonk, http://www.openclonk.org
 *
 * Copyright (c) 2001-2009, RedWolf Design GmbH, http://www.clonk.de/
 * Copyright (c) 2013, The OpenClonk Team and contributors
 *
 * Distributed under the terms of the ISC license; see accompanying file
 * "COPYING" for details.
 *
 * "Clonk" is a registered trademark of Matthes Bender, used with permission.
 * See accompanying file "TRADEMARK" for details.
 *
 * To redistribute this file separately, substitute the full license texts
 * for the above references.
 */

/* Editor windows using Qt - internal state */

#ifndef INC_C4ConsoleQtState
#define INC_C4ConsoleQtState

#ifdef WITH_QT_EDITOR

#include <C4Include.h> // needed for automoc
#include <C4ConsoleGUI.h>
#include <C4ToolsDlg.h>
#include <C4ConsoleQt.h>
#include <ui_C4ConsoleQtMainWindow.h>

#ifdef USE_WIN32_WINDOWS
#include <WinUser.h>
#include <resource.h>
#endif


/* Forward string translation to GetResStr */

class C4ConsoleQtTranslator : public QTranslator
{
	Q_OBJECT
public:
	bool isEmpty() { return false; }
	QString translate(const char * context, const char * sourceText, const char * disambiguation = 0, int n = -1) const;
};

extern C4ConsoleQtTranslator qt_translator;

class C4ConsoleClientAction : public QAction
{
	Q_OBJECT

		int32_t client_id;
public:
	C4ConsoleClientAction(int32_t client_id, const char *text, QObject *parent);
	int32_t GetClientID() const { return client_id; }
	private slots:
	void Execute();
};

class C4ConsoleRemovePlayerAction : public QAction
{
	Q_OBJECT

		int32_t player_num;
public:
	C4ConsoleRemovePlayerAction(int32_t player_num, const char *text, QObject *parent);
	int32_t GetPlayerNum() const { return player_num; }
	private slots:
	void Execute();
};

class C4ConsoleOpenViewportAction : public QAction
{
	Q_OBJECT

		int32_t player_num;
public:
	C4ConsoleOpenViewportAction(int32_t player_num, const char *text, QObject *parent);
	int32_t GetPlayerNum() const { return player_num; }
	private slots:
	void Execute();
};

class C4ConsoleViewportWidget : public QDockWidget
{
	Q_OBJECT

	class C4ViewportWindow *cvp;
	QWindow *window;
	QPalette pal_inactive, pal_active;

protected:
	virtual void focusInEvent(QFocusEvent * event);
	virtual void focusOutEvent(QFocusEvent * event);
	virtual QObject *focusObject() const { return window; }

public:
	C4ConsoleViewportWidget(class QMainWindow *parent, class C4ViewportWindow *window);
	virtual void closeEvent(QCloseEvent * event);
	void OnActiveChanged(bool active);
	class C4ViewportWindow *GetViewportWindow() const { return cvp; }

private slots:
	void DockLocationChanged(Qt::DockWidgetArea new_area);
};

class C4ConsoleQtMainWindow : public QMainWindow
{
	Q_OBJECT
	class C4ConsoleGUIState *state;

public:
	C4ConsoleQtMainWindow(class C4AbstractApp *app, class C4ConsoleGUIState *state);

	void closeEvent(class QCloseEvent *event);

	public slots:
	// Toolbar items
	void PlayPressed(bool down);
	void PausePressed(bool down);
	void CursorGamePressed(bool down);
	void CursorSelectPressed(bool down);
	void CursorCreateObjPressed(bool down);
	void CursorDrawPenPressed(bool down);
	void CursorDrawLinePressed(bool down);
	void CursorDrawRectPressed(bool down);
	void CursorFillPressed(bool down);
	void CursorPickerPressed(bool down);
	void DynamicLandscapePressed(bool down);
	void StaticLandscapePressed(bool down);
	void ExactLandscapePressed(bool down);
	void DrawSizeChanged(int newval);
	// File menu
	void FileOpen();
	void FileOpenWithPlayers();
	void FileRecord();
	void FileSave();
	void FileSaveAs();
	void FileSaveGameAs();
	void FileClose();
	void FileQuit();
	// Player menu
	void PlayerJoin();
	// Window menu
	void ViewportNew();
	// Help menu
	void HelpAbout();
	// Console edits enter pressed events
	void MainConsoleEditEnter(); // console edit below log
	void PropertyConsoleEditEnter(); // console edit of property window
	// View selection changes
	void OnCreatorSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
	void OnCreatorCurrentChanged(const QModelIndex & current, const QModelIndex & previous);
	void OnObjectListSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
};


/* Main Qt editor class controlled through ControlGUI */

class C4ConsoleGUIState // Avoid direct declaration of C4ConsoleGUI::State because qmake doesn't like some declarations in the nested class
{
public:
	std::unique_ptr<QApplication> application;
	std::unique_ptr<C4ConsoleQtMainWindow> window;
	std::unique_ptr<class C4ConsoleQtPropListModel> property_model;
	std::unique_ptr<class C4ConsoleQtObjectListModel> object_list_model;
	std::unique_ptr<class C4ConsoleQtDefinitionListModel> definition_list_model;
	std::list<C4ConsoleViewportWidget *> viewports;
	std::list<std::unique_ptr<C4ConsoleClientAction> > client_actions;
	std::list<std::unique_ptr<C4ConsoleRemovePlayerAction> > player_actions;
	std::list<std::unique_ptr<C4ConsoleOpenViewportAction> > viewport_actions;
	Ui::MainWindow ui;

	// ptrs owned by window
	QMainWindow *viewport_area;
	QLabel *status_cursor, *status_framecounter, *status_timefps;
	QAction *window_menu_separator;

	// Current editor/tool states
	// Cannot use direct members in C4EditorCursor because callbacks into the GUI happen before the values change.
	// If other C4Console implementations are removed, the state could be merged and these members removed.
	bool enabled, recording, net_enabled;
	int32_t landscape_mode, editcursor_mode, drawing_tool;
	StdCopyStrBuf material, texture, back_material, back_texture;

	// Updating states to prevent callbacks on internal selection updates
	int32_t is_object_selection_updating;

	C4ConsoleGUIState(C4ConsoleGUI *console);
	~C4ConsoleGUIState();

	void AddToolbarSpacer(int space);
	bool CreateConsoleWindow(C4AbstractApp *app);
	void Execute(bool redraw_only=false);
	void Redraw() { Execute(true); }
	void UpdateActionStates();
	void UpdateMatTex();
	void UpdateBackMatTex();
	// Set modes and tools
	void SetEnabled(bool to_enabled) { enabled = to_enabled; UpdateActionStates(); if (enabled) ReInitDefinitions(); }
	void SetLandscapeMode(int32_t to_landscape_mode) { landscape_mode = to_landscape_mode; UpdateActionStates(); }
	void SetEditCursorMode(int32_t to_editcursor_mode) { editcursor_mode = to_editcursor_mode; UpdateActionStates(); }
	void SetDrawingTool(int32_t to_drawing_tool) { drawing_tool = to_drawing_tool; UpdateActionStates(); }
	void SetMaterial(const char *new_material) { material.Copy(new_material); UpdateMatTex(); }
	void SetTexture(const char *new_texture) { texture.Copy(new_texture); UpdateMatTex(); }
	void SetBackMaterial(const char *new_material) { back_material.Copy(new_material); UpdateBackMatTex(); }
	void SetBackTexture(const char *new_texture) { back_texture.Copy(new_texture); UpdateBackMatTex(); }
	void SetRecording(bool to_recording) { recording = to_recording; UpdateActionStates(); }
	void SetNetEnabled(bool enabled) { net_enabled = enabled; UpdateActionStates(); }
	
	void AddNetMenuItem(int32_t index, const char *text);
	void ClearNetMenu();
	void AddKickPlayerMenuItem(int32_t plr, const char *text, bool item_enabled);
	void ClearPlayerMenu();
	void AddPlayerViewportMenuItem(int32_t plr, const char *text);
	void ClearViewportMenu();
	void AddViewport(class C4ViewportWindow *cvp);
	void OnViewportActiveChanged(C4ViewportWindow *cvp, bool is_active);
	void SetInputFunctions(std::list<const char*> &functions);
	void PropertyDlgUpdate(C4EditCursorSelection &rSelection, bool force_function_update);
	void ReInitDefinitions();
	void OnCreatorSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
	void SetObjectSelection(class C4EditCursorSelection &rSelection);
	void OnObjectListSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
	void OnCreatorCurrentChanged(const QModelIndex & current, const QModelIndex & previous);
};

class C4ConsoleGUI::State : public C4ConsoleGUIState
{
public:
	State(C4ConsoleGUI *console) : C4ConsoleGUIState(console) {}
};

class C4ToolsDlg::State : C4ConsoleGUI::InternalState<class C4ToolsDlg>
{
public:
	State(C4ToolsDlg *toolsDlg) : C4ConsoleGUI::InternalState<class C4ToolsDlg>(toolsDlg) {}
	~State() {}

	void Clear() {}
	void Default() {}
};

#endif // WITH_QT_EDITOR
#endif // INC_C4ConsoleQtState