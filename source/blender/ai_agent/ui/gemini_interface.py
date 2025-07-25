"""
Gemini AI Interface for Blender
==============================

This module provides the Blender UI for interacting with the Gemini AI integration.
"""

import bpy
import asyncio
import logging
from typing import Dict, Any, Optional, List, Tuple
from ..core.gemini_integration import GeminiIntegration, GeminiMode, GeminiResponse
from ..core.config_manager import ConfigManager

# Set up logger
logger = logging.getLogger(__name__)


class GeminiSuggestion(bpy.types.PropertyGroup):
    """A suggested follow-up question."""
    
    text: bpy.props.StringProperty(
        name="Suggestion",
        description="A suggested follow-up question",
        default=""
    )


class GeminiAddonPreferences(bpy.types.AddonPreferences):
    """Preferences for the Gemini AI add-on."""
    
    bl_idname = __package__
    
    # UI State
    show_api_key: bpy.props.BoolProperty(
        name="Show API Key",
        description="Show or hide the API key field",
        default=False
    )
    
    # Mode
    gemini_mode: bpy.props.EnumProperty(
        name="Mode",
        description="Default interaction mode",
        items=[
            (GeminiMode.MODELING_ASSISTANT.value, "Modeling Assistant", "Get help with 3D modeling"),
            (GeminiMode.CODE_HELPER.value, "Code Helper", "Get help with Python/API code"),
            (GeminiMode.SCENE_ANALYSIS.value, "Scene Analysis", "Analyze and improve your scene"),
            (GeminiMode.WORKFLOW_OPTIMIZER.value, "Workflow Optimizer", "Optimize your workflow"),
            (GeminiMode.TUTORIAL_MODE.value, "Tutorial Mode", "Step-by-step guidance")
        ],
        default=GeminiMode.MODELING_ASSISTANT.value
    )
    
    # API Key (not actually used for storage, just for the UI)
    gemini_api_key: bpy.props.StringProperty(
        name="API Key",
        description="Your Google Gemini API key (stored securely in keyring)",
        subtype='PASSWORD',
        update=lambda self, context: self._on_api_key_updated()
    )
    
    # Advanced settings
    show_advanced: bpy.props.BoolProperty(
        name="Advanced Settings",
        description="Show advanced settings",
        default=False
    )
    
    temperature: bpy.props.FloatProperty(
        name="Creativity",
        description="Response creativity (0.0 = precise, 1.0 = creative)",
        default=0.7,
        min=0.0,
        max=1.0,
        subtype='FACTOR',
        update=lambda self, context: self._on_setting_updated("temperature", self.temperature)
    )
    
    max_tokens: bpy.props.IntProperty(
        name="Max Tokens",
        description="Maximum number of tokens to generate",
        default=2048,
        min=1,
        max=8192,
        update=lambda self, context: self._on_setting_updated("max_tokens", self.max_tokens)
    )
    
    def _on_api_key_updated(self):
        """Handle API key updates."""
        config = ConfigManager.get_instance()
        config.set_api_key("gemini", self.gemini_api_key, save_to_keyring=True)
    
    def _on_setting_updated(self, setting_name: str, value: Any):
        """Handle updates to settings."""
        config = ConfigManager.get_instance()
        config.set_setting("gemini", setting_name, value)
    
    def draw(self, context):
        layout = self.layout
        config = ConfigManager.get_instance()
        
        # API Key section
        box = layout.box()
        box.label(text="API Configuration", icon='PREFERENCES')
        
        # Toggle for showing API key
        row = box.row()
        row.prop(self, "show_api_key", 
                icon='TRIA_DOWN' if self.show_api_key else 'TRIA_RIGHT',
                icon_only=True, emboss=False)
        row.label(text="API Key Configuration")
        
        # API key input (only shown if expanded)
        if self.show_api_key:
            # Get the current API key from secure storage
            current_key = config.get_api_key("gemini")
            if current_key:
                # Show a masked version of the key
                masked_key = f"•••••{current_key[-4:]}" if len(current_key) > 4 else "••••"
                row = box.row()
                row.label(text=f"Current Key: {masked_key}")
                row.operator("gemini.clear_api_key", text="Clear Key", icon='X')
            else:
                # No key set, show input field
                box.prop(self, "gemini_api_key", text="")
                box.label(text="Key will be stored securely in your system keyring", icon='INFO')
        
        # Mode selection
        box = layout.box()
        box.label(text="Default Settings", icon='SETTINGS')
        box.prop(self, "gemini_mode")
        
        # Advanced settings
        box = layout.box()
        box.prop(self, "show_advanced", 
                icon='TRIA_DOWN' if self.show_advanced else 'TRIA_RIGHT',
                icon_only=True, emboss=False)
        box.label(text="Advanced Settings")
        
        if self.show_advanced:
            box.prop(self, "temperature")
            box.prop(self, "max_tokens")


class GEMINI_OT_ClearAPIKey(bpy.types.Operator):
    """Clear the stored API key."""
    bl_idname = "gemini.clear_api_key"
    bl_label = "Clear API Key"
    bl_options = {'INTERNAL'}
    
    def execute(self, context):
        config = ConfigManager.get_instance()
        config.delete_api_key("gemini")
        
        # Clear the UI field
        prefs = context.preferences.addons[__package__].preferences
        prefs.gemini_api_key = ""
        
        self.report({'INFO'}, "API key cleared")
        return {'FINISHED'}


class GEMINI_OT_TestConnection(bpy.types.Operator):
    """Test connection to Gemini API"""
    bl_idname = "gemini.test_connection"
    bl_label = "Test Connection"
    bl_description = "Test connection to Gemini API"
    
    def execute(self, context):
        # Get the API key from secure storage
        config = ConfigManager.get_instance()
        api_key = config.get_api_key("gemini")
        
        if not api_key:
            self.report({'ERROR'}, "No API key configured")
            return {'CANCELLED'}
        
        # Create a new async task to test the connection
        task = asyncio.ensure_future(self._test_connection_async(api_key, context))
        
        # Store the task in the window manager
        context.window_manager.gemini_test_task = task
        
        # Add a modal handler to check for task completion
        context.window_manager.modal_handler_add(self)
        
        # Show a progress indicator
        context.window_manager.gemini_processing = True
        
        return {'RUNNING_MODAL'}
    
    async def _test_connection_async(self, api_key: str, context):
        """Test the connection to the Gemini API asynchronously."""
        gemini = GeminiIntegration(api_key=api_key)
        
        try:
            # Initialize the Gemini integration
            if not await gemini.initialize():
                self.report({'ERROR'}, "Failed to initialize Gemini integration")
                return False
            
            # Test the connection with a simple query
            response = await gemini.process_query("Hello, can you hear me?")
            
            if response and response.content:
                self.report({'INFO'}, "✅ Connection successful!")
                return True
            else:
                self.report({'WARNING'}, "⚠️ Received empty response")
                return False
        
        except Exception as e:
            logger.error(f"Connection test failed: {str(e)}", exc_info=True)
            self.report({'ERROR'}, f"❌ Connection failed: {str(e)}")
            return False
        
        finally:
            # Clean up
            await gemini.close()
    
    def modal(self, context, event):
        """Modal operator to check for task completion."""
        if not hasattr(context.window_manager, 'gemini_test_task'):
            return {'CANCELLED'}
        
        task = context.window_manager.gemini_test_task
        
        if task.done():
            # Task is complete, clean up
            del context.window_manager.gemini_test_task
            context.window_manager.gemini_processing = False
            
            # Check for exceptions
            if task.exception():
                logger.error(f"Error in connection test: {str(task.exception())}")
                self.report({'ERROR'}, f"Error: {str(task.exception())}")
                return {'CANCELLED'}
            
            return {'FINISHED'}
        
        return {'PASS_THROUGH'}


class GEMINI_PT_panel(bpy.types.Panel):
    """Main panel for the Gemini AI Assistant in the 3D View sidebar"""
    bl_label = "Gemini AI Assistant"
    bl_idname = "GEMINI_PT_panel"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = 'AI Assistant'

    def draw(self, context):
        layout = self.layout
        prefs = context.preferences.addons[__package__].preferences
        scene = context.scene
        
        # Check if API key is configured
        config = ConfigManager.get_instance()
        has_api_key = bool(config.get_api_key("gemini"))
        
        # API Key status
        box = layout.box()
        row = box.row()
        row.label(text="Status: ")
        
        if has_api_key:
            row.label(text="✅ API Key Configured", icon='CHECKMARK')
            row.operator("gemini.test_connection", text="", icon='FILE_REFRESH')
        else:
            row.label(text="❌ No API Key", icon='ERROR')
            row.operator("gemini.test_connection", text="Test")
        
        # Mode selection
        box = layout.box()
        box.label(text="Mode", icon='MODIFIER')
        
        # Create a grid for mode selection
        grid = box.grid_flow(columns=2, even_columns=True, even_rows=True)
        
        for mode in GeminiMode:
            desc = GeminiMode.get_mode_description(mode)
            op = grid.operator(
                "gemini.set_mode", 
                text=desc, 
                icon=self._get_mode_icon(mode.value),
                depress=(prefs.gemini_mode == mode.value)
            )
            op.mode = mode.value
        
        # Query input
        box = layout.box()
        box.label(text="Ask Gemini", icon='CONSOLE')
        
        # Multi-line text input
        box.prop(
            scene, 
            "gemini_query", 
            text="", 
            icon='QUESTION',
            icon_value=0
        )
        
        # Action buttons
        row = box.row(align=True)
        
        # Submit button
        submit_op = row.operator(
            "gemini.submit_query", 
            text="Submit" if not hasattr(scene, "gemini_last_query") or not scene.gemini_last_query else "Ask Again", 
            icon='PLAY'
        )
        submit_op.is_retry = False
        
        # Retry button if there's a previous query
        if hasattr(scene, "gemini_last_query") and scene.gemini_last_query:
            retry_op = row.operator(
                "gemini.submit_query", 
                text="Retry", 
                icon='FILE_REFRESH'
            )
            retry_op.is_retry = True
        
        # Test connection button
        test_op = row.operator(
            "gemini.test_connection", 
            text="", 
            icon='WORLD'
        )
        test_op.show_message = False
        
        # Show processing indicator
        if hasattr(context.window_manager, 'gemini_processing') and context.window_manager.gemini_processing:
            box = layout.box()
            row = box.row()
            row.label(text="Processing...")
            row.operator("gemini.cancel_query", text="", icon='CANCEL')
        
        # Response area
        if hasattr(scene, "gemini_response") and scene.gemini_response:
            self._draw_response(context, layout)
    
    def _draw_response(self, context, layout):
        """Draw the response area with the AI's response."""
        scene = context.scene
        box = layout.box()
        box.label(text="Response", icon='TEXT')
        
        # Display response with word wrap
        response_text = scene.gemini_response
        
        # Handle code blocks specially
        if "```" in response_text:
            parts = response_text.split("```")
            for i, part in enumerate(parts):
                if i % 2 == 0:
                    # Regular text
                    for line in part.split('\n'):
                        if line.strip():
                            box.label(text=line)
                        else:
                            box.separator()
                else:
                    # Code block
                    code_box = box.box()
                    code_box.alert = True
                    
                    # Add a copy button for code blocks
                    row = code_box.row()
                    row.label(text="", icon='COPYDOWN')
                    op = row.operator(
                        "gemini.copy_to_clipboard",
                        text="Copy Code"
                    )
                    op.text = part.strip()
                    
                    # Display the code
                    for line in part.strip().split('\n'):
                        code_box.label(text=f"    {line}")
                    
                    box.separator()
        else:
            # Simple text response
            for line in response_text.split('\n'):
                if line.strip():
                    box.label(text=line)
                else:
                    box.separator()
        
        # Show suggestions if available
        if hasattr(scene, "gemini_suggestions") and scene.gemini_suggestions:
            box = layout.box()
            box.label(text="You Might Also Ask:", icon='LIGHT')
            
            for suggestion in scene.gemini_suggestions:
                op = box.operator(
                    "gemini.use_suggestion",
                    text=suggestion.text,
                    icon='DOT'
                )
                op.suggestion = suggestion.text
    
    def _get_mode_icon(self, mode: str) -> str:
        """Get the icon for a mode."""
        icons = {
            GeminiMode.MODELING_ASSISTANT.value: 'MESH_CUBE',
            GeminiMode.CODE_HELPER.value: 'TEXT',
            GeminiMode.SCENE_ANALYSIS.value: 'SCENE_DATA',
            GeminiMode.WORKFLOW_OPTIMIZER.value: 'GRAPH',
            GeminiMode.TUTORIAL_MODE.value: 'HELP'
        }
        return icons.get(mode, 'QUESTION')
        layout.label(text="Ask Gemini:")
        layout.prop(scene, "gemini_query", text="")
        
        # Submit button


class GEMINI_OT_SetMode(bpy.types.Operator):
    """Set the interaction mode for the Gemini AI."""
    bl_idname = "gemini.set_mode"
    bl_label = "Set Mode"
    bl_options = {'INTERNAL'}
    
    mode: bpy.props.StringProperty(
        name="Mode",
        description="The interaction mode to set",
        default=GeminiMode.MODELING_ASSISTANT.value
    )
    
    def execute(self, context):
        prefs = context.preferences.addons[__package__].preferences
        prefs.gemini_mode = self.mode
        logger.info(f"Set Gemini mode to: {self.mode}")
        return {'FINISHED'}


class GEMINI_OT_SubmitQuery(bpy.types.Operator):
    """Submit a query to the Gemini AI."""
    bl_idname = "gemini.submit_query"
    bl_label = "Submit Query"
    bl_description = "Send the query to Gemini AI"
    
    is_retry: bpy.props.BoolProperty(
        name="Is Retry",
        description="Whether this is a retry of the last query",
        default=False
    )
    
    def execute(self, context):
        scene = context.scene
        
        # If this is a retry, use the last query
        if self.is_retry and hasattr(scene, "gemini_last_query"):
            query = scene.gemini_last_query
        else:
            # Get the query from the scene property
            query = scene.gemini_query
            if not query.strip():
                self.report({'WARNING'}, "Please enter a query")
                return {'CANCELLED'}
            
            # Store the query for potential retry
            scene.gemini_last_query = query
        
        # Get the API key from secure storage
        config = ConfigManager.get_instance()
        api_key = config.get_api_key("gemini")
        
        if not api_key:
            self.report({'ERROR'}, "Please enter your Gemini API key in the add-on preferences")
            return {'CANCELLED'}
        
        # Get the selected mode from preferences
        prefs = context.preferences.addons[__package__].preferences
        mode = GeminiMode(prefs.gemini_mode)
        
        # Clear previous response and suggestions
        if hasattr(scene, "gemini_response"):
            scene.gemini_response = ""
        
        if hasattr(scene, "gemini_suggestions"):
            scene.gemini_suggestions.clear()
        
        # Create a new async task to process the query
        task = asyncio.ensure_future(
            self._process_query_async(api_key, mode, query, context)
        )
        
        # Store the task in the window manager
        context.window_manager.gemini_task = task
        context.window_manager.gemini_processing = True
        
        # Add a modal handler to check for task completion
        context.window_manager.modal_handler_add(self)
        
        return {'RUNNING_MODAL'}
    
    async def _process_query_async(self, api_key: str, mode: GeminiMode, query: str, context):
        """Process the query asynchronously."""
        gemini = GeminiIntegration(api_key=api_key, mode=mode)
        
        try:
            # Initialize the Gemini integration
            if not await gemini.initialize():
                self.report({'ERROR'}, "Failed to initialize Gemini integration")
                return
            
            # Get scene context for 3D modeling assistance
            scene_context = self._get_scene_context(context)
            
            # Process the query with streaming
            response = await gemini.process_query(
                query=query,
                context=scene_context,
                stream=True  # Enable streaming for better UX
            )
            
            # Store the response in the scene
            context.scene.gemini_response = response.content
            
            # Store suggestions if any
            if hasattr(response, 'suggestions') and response.suggestions:
                if not hasattr(context.scene, "gemini_suggestions"):
                    context.scene.gemini_suggestions = []
                
                context.scene.gemini_suggestions.clear()
                for suggestion in response.suggestions[:3]:  # Limit to 3 suggestions
                    context.scene.gemini_suggestions.add().text = suggestion
            
            # Update the UI
            self._redraw_ui(context)
            
        except Exception as e:
            logger.error(f"Error processing query: {str(e)}", exc_info=True)
            self.report({'ERROR'}, f"Error processing query: {str(e)}")
        
        finally:
            # Clean up
            await gemini.close()
            context.window_manager.gemini_processing = False
    
    def _get_scene_context(self, context) -> Dict[str, Any]:
        """Get relevant context from the current Blender scene."""
        scene = context.scene
        
        # Get selected objects
        selected_objects = [
            {
                "name": obj.name,
                "type": obj.type,
                "vertices": len(obj.data.vertices) if hasattr(obj.data, 'vertices') else 0,
                "materials": [mat.name for mat in obj.material_slots] if hasattr(obj, 'material_slots') else []
            }
            for obj in context.selected_objects
        ]
        
        # Get scene info
        scene_info = {
            "frame_current": scene.frame_current,
            "frame_start": scene.frame_start,
            "frame_end": scene.frame_end,
            "render_engine": scene.render.engine,
            "selected_objects": selected_objects,
            "mode": context.mode
        }
        
        return scene_info
    
    def _redraw_ui(self, context):
        """Force a redraw of the UI."""
        for window in context.window_manager.windows:
            for area in window.screen.areas:
                if area.type == 'VIEW_3D':
                    area.tag_redraw()
    
    def modal(self, context, event):
        """Modal operator to check for task completion."""
        if not hasattr(context.window_manager, 'gemini_task'):
            return {'CANCELLED'}
        
        task = context.window_manager.gemini_task
        
        if task.done():
            # Task is complete, clean up
            del context.window_manager.gemini_task
            
            # Check for exceptions
            if task.exception():
                logger.error(f"Error in task: {str(task.exception())}", exc_info=True)
                self.report({'ERROR'}, f"Error: {str(task.exception())}")
                return {'CANCELLED'}
            
            return {'FINISHED'}
        
        return {'PASS_THROUGH'}


class GEMINI_OT_UseSuggestion(bpy.types.Operator):
    """Use a suggested query."""
    bl_idname = "gemini.use_suggestion"
    bl_label = "Use Suggestion"
    bl_options = {'INTERNAL'}
    
    suggestion: bpy.props.StringProperty(
        name="Suggestion",
        description="The suggested query to use",
        default=""
    )
    
    def execute(self, context):
        # Set the suggestion as the current query
        context.scene.gemini_query = self.suggestion
        
        # Submit the query
        bpy.ops.gemini.submit_query()
        
        return {'FINISHED'}


class GEMINI_OT_CopyToClipboard(bpy.types.Operator):
    """Copy text to the clipboard."""
    bl_idname = "gemini.copy_to_clipboard"
    bl_label = "Copy to Clipboard"
    bl_options = {'INTERNAL'}
    
    text: bpy.props.StringProperty(
        name="Text",
        description="The text to copy to the clipboard",
        default=""
    )
    
    def execute(self, context):
        # Copy the text to the clipboard
        context.window_manager.clipboard = self.text
        self.report({'INFO'}, "Code copied to clipboard")
        return {'FINISHED'}


class GEMINI_OT_CancelQuery(bpy.types.Operator):
    """Cancel the current query."""
    bl_idname = "gemini.cancel_query"
    bl_label = "Cancel Query"
    bl_options = {'INTERNAL'}
    
    def execute(self, context):
        # Check if there's an active task
        if hasattr(context.window_manager, 'gemini_task'):
            task = context.window_manager.gemini_task
            if not task.done():
                task.cancel()
                
            # Clean up
            del context.window_manager.gemini_task
        
        # Reset the processing flag
        if hasattr(context.window_manager, 'gemini_processing'):
            context.window_manager.gemini_processing = False
        
        self.report({'INFO'}, "Query cancelled")
        return {'FINISHED'}


def register():
    """Register all classes and properties."""
    from bpy.utils import register_class
    
    # Register property groups first
    register_class(GeminiSuggestion)
    register_class(GeminiAddonPreferences)
    
    # Register operators and panels
    classes = [
        GEMINI_OT_CancelQuery,
        GEMINI_OT_CopyToClipboard,
        GEMINI_OT_UseSuggestion,
        GEMINI_OT_SubmitQuery,
        GEMINI_OT_SetMode,
        GEMINI_OT_TestConnection,
        GEMINI_PT_panel
    ]
    
    for cls in classes:
        register_class(cls)
    
    # Register scene properties
    bpy.types.Scene.gemini_query = bpy.props.StringProperty(
        name="Query",
        description="Enter your question for Gemini AI",
        default=""
    )
    
    bpy.types.Scene.gemini_last_query = bpy.props.StringProperty(
        name="Last Query",
        description="The last query that was submitted",
        default=""
    )
    
    bpy.types.Scene.gemini_response = bpy.props.StringProperty(
        name="Response",
        description="Response from Gemini AI",
        default=""
    )
    
    bpy.types.Scene.gemini_suggestions = bpy.props.CollectionProperty(
        type=GeminiSuggestion,
        name="Suggestions",
        description="Suggested follow-up questions"
    )
    
    # Add a custom property to track if a query is being processed
    bpy.types.WindowManager.gemini_processing = bpy.props.BoolProperty(
        name="Gemini Processing",
        description="Whether a Gemini query is currently being processed",
        default=False
    )
    
    logger.info("Gemini AI interface registered")


def unregister():
    """Unregister all classes and properties."""
    from bpy.utils import unregister_class
    from bpy.types import Scene, WindowManager
    
    # Unregister in reverse order
    classes = [
        GEMINI_PT_panel,
        GEMINI_OT_CancelQuery,
        GEMINI_OT_CopyToClipboard,
        GEMINI_OT_UseSuggestion,
        GEMINI_OT_SubmitQuery,
        GEMINI_OT_SetMode,
        GEMINI_OT_TestConnection,
        GeminiAddonPreferences,
        GeminiSuggestion
    ]
    
    for cls in classes:
        if hasattr(bpy.types, cls.__name__):
            unregister_class(cls)
    
    # Remove properties
    if hasattr(Scene, "gemini_query"):
        del Scene.gemini_query
    if hasattr(Scene, "gemini_last_query"):
        del Scene.gemini_last_query
    if hasattr(Scene, "gemini_response"):
        del Scene.gemini_response
    if hasattr(Scene, "gemini_suggestions"):
        del Scene.gemini_suggestions
    if hasattr(WindowManager, "gemini_processing"):
        del WindowManager.gemini_processing
    
    logger.info("Gemini AI interface unregistered")
    bpy.utils.unregister_class(GeminiProperties)


if __name__ == "__main__":
    register()
