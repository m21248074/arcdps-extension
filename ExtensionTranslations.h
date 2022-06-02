﻿#pragma once

#include <array>

enum ExtensionTranslation {
	ET_Left,
	ET_Right,
	ET_Center,
	ET_Unaligned,
	ET_PositionManual,
	ET_PositionScreenRelative,
	ET_PositionWindowRelative,
	ET_Unknown,
	ET_CornerPositionTopLeft,
	ET_CornerPositionTopRight,
	ET_CornerPositionBottomLeft,
	ET_CornerPositionBottomRight,
	ET_SizingPolicySizeToContent,
	ET_SizingPolicySizeContentToWindow,
	ET_SizingPolicyManualWindowSize,
	ET_KeyInputPopupName,
	ET_ApplyButton,
	ET_CancelButton,
	ET_UpdateDesc,
	ET_UpdateCurrentVersion,
	ET_UpdateNewVersion,
	ET_UpdateOpenPage,
	ET_UpdateAutoButton,
	ET_UpdateInProgress,
	ET_UpdateRestartPending,
	ET_UpdateError,
	ET_Style,
	ET_TitleBar,
	ET_Background,
	ET_Scrollbar,
	ET_Padding,
	ET_SizingPolicy,
	ET_AppearAsInOption,
	ET_TitleBarText,
	ET_Shortcut,
	ET_Position,
	ET_FromAnchorPanelCorner,
	ET_ThisPanelCorner,
	ET_AnchorWindow,
	ET_ColumnSetup,
	ET_ShowBasedOnMap,
	ET_AlternatingRowBg,
	ET_HighlightHoveredRow,
	ET_MaxDisplayed,
	ET_HeaderAlignment,
	ET_ColumnAlignment,
	ET_Language,
	ET_SettingsShowHeaderText,
};

constexpr auto EXTENSION_TRANSLATION_ENGLISH = std::to_array({
	"Left",	            // ET_Left,
	"Right",	        //     ET_Right,
	"Centered",	        //     ET_Center,
	"Standard",	        //     ET_Unaligned,
	"Manual",	        //     ET_PositionManual,
	"Screen Relative",	//     ET_PositionScreenRelative,
	"Window Relative",	// 	ET_PositionWindowRelative,
	"Unknown",	        //     ET_Unknown,
	"Top-Left",	        //     ET_CornerPositionTopLeft,
	"Top-Right",        //     ET_CornerPositionTopRight,
	"Bottom-Left",	    //     ET_CornerPositionBottomLeft,
	"Bottom-Right",	    //     ET_CornerPositionBottomRight,
	"Size to Content",	//     ET_SizingPolicySizeToContent,
	"Size Content to Window",	//     ET_SizingPolicySizeContentToWindow,
	"Manual Window Size",	//     ET_SizingPolicyManualWindowSize,
	"KeyBind",	        //     ET_KeyInputPopupName,
	"Apply",	        //     ET_ApplyButton,
	"Cancel",	        //     ET_CancelButton,
	"A new update for the {} is available.",	//     ET_UpdateDesc,
	"Current version",	//     ET_UpdateCurrentVersion,
	"New Version",	    //     ET_UpdateNewVersion,
	"Open download page",	//     ET_UpdateOpenPage,
	"Update automatically",	//     ET_UpdateAutoButton,
	"Autoupdate in progress",	//     ET_UpdateInProgress,
	"Autoupdate finished, restart your game to activate it.",	//     ET_UpdateRestartPending,
	"Autoupdate error, please update manually.",	//     ET_UpdateError,
	"Style", // ET_Style
	"Title bar", // ET_TitleBar
	"Background", // ET_Background
	"Scrollbar", // ET_Scrollbar
	"Padding", // ET_Padding
	"Sizing Policy", // ET_SizingPolicy
	"Appear as in option", // ET_AppearAsInOption
	"Title bar", // ET_TitleBarText
	"Shortcut", // ET_Shortcut
	"Position", // ET_Position
	"From anchor panel corner", // ET_FromAnchorPanelCorner
	"This panel corner", // ET_ThisPanelCorner
	"Anchor window", // ET_AnchorWindow
	"Column Setup", // ET_ColumnSetup
	"Show Columns based on map", // ET_ShowBasedOnMap
	"Alternating Row Background", // ET_AlternatingRowBg
	"Highlight hovered row", // ET_HighlightHoveredRow
	"max displayed", // ET_MaxDisplayed
	"Header Alignment", // ET_HeaderAlignment
	"Column Alignment", // ET_ColumnAlignment
	"Language", // ET_Language
	"Show header with text instead of images", // ET_SettingsShowHeaderText
});

constexpr auto EXTENSION_TRANSLATION_GERMAN = std::to_array({
	"Links",		// ET_Left,
	"Rechts",		// ET_Right,
	"Zentriert",	// ET_Center,
	"Standard",		// ET_Unaligned,
	"Manuell",		// ET_PositionManual,
	"Relativ zum Bildschirm",	// ET_PositionScreenRelative,
	"Relativ zu einem anderen Fenster",	// ET_PositionWindowRelative,
	"Unbekannt",	// ET_Unknown,
	"Oben-Links",	// ET_CornerPositionTopLeft,
	"Oben-Rechts",	// ET_CornerPositionTopRight,
	"Unten-Links",	// ET_CornerPositionBottomLeft,
	"Unten-Rechts",	// ET_CornerPositionBottomRight,
	"Passe Fenster an Inhalt an",	// ET_SizingPolicySizeToContent,
	"Passe Inhalt an Fenster an",	// ET_SizingPolicySizeContentToWindow,
	"Manuelle Fenstergröße",	// ET_SizingPolicyManualWindowSize,
	"Tastenbelegung",	// ET_KeyInputPopupName,
	"Anwenden",		// ET_ApplyButton,
	"Abbrechen",	// ET_CancelButton,
	"Eine neue Version für das {} ist verfügbar.",	// ET_UpdateDesc,
	"Aktuelle Version", // ET_UpdateCurrentVersion,
	"Neue Version",	// ET_UpdateNewVersion,
	"Öffne Download Seite",	// ET_UpdateOpenPage,
	"Automatisch Aktualisieren",	// ET_UpdateAutoButton,
	"Aktualisierung im Gange",	// ET_UpdateInProgress,
	"Aktualisierung beendet, starte das Spiel neu zum Aktivieren.",	// ET_UpdateRestartPending,
	"Aktualisierung fehlgeschlagen, bitte update manuell.",	// ET_UpdateError,
	"Style", // ET_Style
	"Titelleiste", // ET_TitleBar
	"Hintergrund", // ET_Background
	"Scrollleiste", // ET_Scrollbar
	"Padding", // ET_Padding
	"Größenregeln", // ET_SizingPolicy
	"Optionstext", // ET_AppearAsInOption
	"Titelleiste Text", // ET_TitleBarText
	"Tastenkürzel", // ET_Shortcut
	"Position", // ET_Position
	"Ecke des anzuheftenden Fensters", // ET_FromAnchorPanelCorner
	"Ecke des aktuellen Fensters", // ET_ThisPanelCorner
	"Anzuheftendes Fenster", // ET_AnchorWindow
	"Spalteneinstellung", // ET_ColumnSetup
	"Zeige Spalten basierend auf der aktuellen Karte.", // ET_ShowBasedOnMap
	"Abwechselnder Zeilenhintergrund", // ET_AlternatingRowBg
	"Markiere die aktuelle Zeile", // ET_HighlightHoveredRow
	"Maximale Anzahl an Zeilen", // ET_MaxDisplayed
	"Ausrichtung der Kopfzeile", // ET_HeaderAlignment
	"Ausrichtung des Inhalts", // ET_ColumnAlignment
	"Sprache", // ET_Language
	"Zeige Text anstatt von Icons in der Kopfzeile", // ET_SettingsShowHeaderText
});

constexpr auto EXTENSION_TRANSLATION_FRENCH = std::to_array({
		 "Gauche",	            // ET_Left,
	"Droit",	        //     ET_Right,
	"Centré",	        //     ET_Center,
	"Standard",	        //     ET_Unaligned,
	"Manuel",	        //     ET_PositionManual,
	"Écran Relatif",	//     ET_PositionScreenRelative,
	"Fenêtre relative",	// 	ET_PositionWindowRelative,
	"Inconnu",	        //     ET_Unknown,
	"Coin supérieur gauche",	        //     ET_CornerPositionTopLeft,
	"Coin supérieur droit",        //     ET_CornerPositionTopRight,
	"Coin inférieur gauche",	    //     ET_CornerPositionBottomLeft,
	"Coin inférieur droit",	    //     ET_CornerPositionBottomRight,
	"Taille à contenu",	//     ET_SizingPolicySizeToContent,
	"Taille du contenu par rapport à la fenêtre",	//     ET_SizingPolicySizeContentToWindow,
	"Taille de la fenêtre Manuel",	//     ET_SizingPolicyManualWindowSize,
	"Lier les touches",	        //     ET_KeyInputPopupName,
	"Appliquer",	        //     ET_ApplyButton,
	"Annuler",	        //     ET_CancelButton,
	"Une nouvelle mise à jour pour le {} est disponible.",	//     ET_UpdateDesc,
	"Version actuelle",	//     ET_UpdateCurrentVersion,
	"Nouvelle version",	    //     ET_UpdateNewVersion,
	"Ouvrir la page de téléchargement",	//     ET_UpdateOpenPage,
	"Mise à jour automatique",	//     ET_UpdateAutoButton,
	"Mise à jour automatique en cours",	//     ET_UpdateInProgress,
	"La mise à jour automatique est terminée, redémarrez votre jeu pour l'activer.",	//     ET_UpdateRestartPending,
	"Erreur de mise à jour automatique, veuillez mettre à jour manuellement.",	//     ET_UpdateError,
	"Style", // ET_Style
	"Barre de titre", // ET_TitleBar
	"Fond d'écran", // ET_Background
	"Barre de défilement", // ET_Scrollbar
	"Padding", // ET_Padding
	"Politique de dimensionnement", // ET_SizingPolicy
	"Apparaître comme dans l'option", // ET_AppearAsInOption
	"Barre de titre", // ET_TitleBarText
	"Raccourci", // ET_Shortcut
	"Position", // ET_Position
	"Depuis le coin du panneau d'ancrage", // ET_FromAnchorPanelCorner
	"Ce coin de panneau", // ET_ThisPanelCorner
	"Fenêtre d'ancrage", // ET_AnchorWindow
	"Configuration des colonnes", // ET_ColumnSetup
	"Afficher les colonnes en fonction de la carte", // ET_ShowBasedOnMap
	"Fond à rangs alternés", // ET_AlternatingRowBg
	"Mettre en évidence la ligne survolée", // ET_HighlightHoveredRow
	"max affiché", // ET_MaxDisplayed
	"Alignement de l'en-tête", // ET_HeaderAlignment
	"Alignement des colonnes", // ET_ColumnAlignment
	"Langue", // ET_Language
	"Afficher l'en-tête avec du texte au lieu des images", // ET_SettingsShowHeaderText
 });

constexpr auto EXTENSION_TRANSLATION_SPANISH = std::to_array({
	"Izquierda",	            // ET_Left,
	"Derecha",	        //     ET_Right,
	"Centrado",	        //     ET_Center,
	"Estándar",	        //     ET_Unaligned,
    "Manual",	        //     ET_PositionManual,
    "Relativo a la pantalla",	//     ET_PositionScreenRelative,
    "Relativo a la ventana",	// 	ET_PositionWindowRelative,
    "Desconocido",	        //     ET_Unknown,
    "Arriba a la izquierda",	        //     ET_CornerPositionTopLeft,
    "Arriba a la derecha",        //     ET_CornerPositionTopRight,
    "Abajo a la izquierda",	    //     ET_CornerPositionBottomLeft,
    "Abajo a la derecha",	    //     ET_CornerPositionBottomRight,
	"Tamaño al contenido",	//     ET_SizingPolicySizeToContent,
    "Tamaño del contenido a la ventana",	//     ET_SizingPolicySizeContentToWindow,
    "Tamaño de la ventana de Manuel",	//     ET_SizingPolicyManualWindowSize,
    "Encuadernación de teclas",	        //     ET_KeyInputPopupName,
    "Aplicar",	        //     ET_ApplyButton,
    "Cancelar",	        //     ET_CancelButton,
    "Está disponible una nueva actualización para el {}.",	//     ET_UpdateDesc,
    "Versión actual",	//     ET_UpdateCurrentVersion,
    "Nueva versión",	    //     ET_UpdateNewVersion,
    "Abrir la página de descarga",	//     ET_UpdateOpenPage,
	"Actualizar automáticamente",	//     ET_UpdateAutoButton,
    "Actualización automática en curso",	//     ET_UpdateInProgress,
    "La actualización automática ha finalizado, reinicia el juego para activarla.",	//     ET_UpdateRestartPending,
    "Error de actualización automática, por favor, actualice manualmente.",	//     ET_UpdateError,
    "Estilo", // ET_Style
    "Barra de título", // ET_TitleBar
    "Antecedentes", // ET_Background
    "Barra de desplazamiento", // ET_Scrollbar
    "Padding", // ET_Padding
    "Política de tallas", // ET_SizingPolicy
    "Aparecer como en la opción", // ET_AppearAsInOption
    "Barra de título", // ET_TitleBarText
    "Atajo", // ET_Shortcut
    "Posición", // ET_Position
    "Desde la esquina del panel de anclaje", // ET_FromAnchorPanelCorner
    "Esta esquina del panel", // ET_ThisPanelCorner
    "Ventana de anclaje", // ET_AnchorWindow
    "Configuración de la columna", // ET_ColumnSetup
    "Mostrar columnas basadas en el mapa", // ET_ShowBasedOnMap
    "Fondo de filas alternas", // ET_AlternatingRowBg
    "Resaltar la fila que se ha desplazado", // ET_HighlightHoveredRow
    "máximo mostrado", // ET_MaxDisplayed
    "Alineación de la cabecera", // ET_HeaderAlignment
    "Alineación de columnas", // ET_ColumnAlignment
    "Idioma", // ET_Language
    "Mostrar la cabecera con texto en lugar de imágenes", // ET_SettingsShowHeaderText
 });

#if __has_include("magic_enum.hpp")
#include <magic_enum.hpp>
static_assert(EXTENSION_TRANSLATION_ENGLISH.size() == magic_enum::enum_count<ExtensionTranslation>());
#endif
static_assert(EXTENSION_TRANSLATION_ENGLISH.size() == EXTENSION_TRANSLATION_GERMAN.size());
static_assert(EXTENSION_TRANSLATION_ENGLISH.size() == EXTENSION_TRANSLATION_FRENCH.size());
static_assert(EXTENSION_TRANSLATION_ENGLISH.size() == EXTENSION_TRANSLATION_SPANISH.size());
