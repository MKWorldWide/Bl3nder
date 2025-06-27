"""
AthenaMist AI Agent for Blender
==============================

A revolutionary AI assistant for Blender that integrates AthenaMist AI capabilities
with an immersive, beautiful interface inspired by creative Skyrim mods.

This add-on provides:
- Natural language understanding for 3D creation
- Creative AI assistance with multiple personality modes
- Immersive UI with themes and animations
- Workflow automation and optimization
- Artistic guidance and technical expertise
"""

bl_info = {
    "name": "AthenaMist AI Agent",
    "author": "AthenaMist Team",
    "version": (1, 0, 0),
    "blender": (3, 0, 0),
    "location": "View3D > Sidebar > AthenaMist",
    "description": "Revolutionary AI assistant for Blender with immersive interface",
    "warning": "Experimental - AI features require internet connection",
    "doc_url": "https://github.com/athenamist/blender-ai-agent",
    "category": "Interface",
}

import bpy
import sys
import os
from pathlib import Path

# Add the ai_agent directory to Python path
current_dir = Path(__file__).parent
if str(current_dir) not in sys.path:
    sys.path.append(str(current_dir))

# Import our modules
from .core.athena_mist_integration import AthenaMistIntegration, AthenaMistMode
from .core.intent_parser import IntentParser
from .ui.immersive_interface import register as register_ui, unregister as unregister_ui


class AthenaMistPreferences(bpy.types.AddonPreferences):
    """Preferences for the AthenaMist AI Agent add-on."""
    
    bl_idname = __package__
    
    # API Configuration
    api_key: bpy.props.StringProperty(
        name="AthenaMist API Key",
        description="Your AthenaMist API key for advanced AI features",
        default="",
        subtype='PASSWORD'
    )
    
    api_base_url: bpy.props.StringProperty(
        name="API Base URL",
        description="Base URL for AthenaMist API",
        default="https://api.athenamist.ai"
    )
    
    # Feature toggles
    enable_ai_features: bpy.props.BoolProperty(
        name="Enable AI Features",
        description="Enable advanced AI capabilities",
        default=True
    )
    
    enable_immersive_ui: bpy.props.BoolProperty(
        name="Enable Immersive UI",
        description="Enable immersive interface features",
        default=True
    )
    
    enable_workflow_automation: bpy.props.BoolProperty(
        name="Enable Workflow Automation",
        description="Enable automated workflow features",
        default=True
    )
    
    # Performance settings
    max_conversation_history: bpy.props.IntProperty(
        name="Max Conversation History",
        description="Maximum number of messages to keep in history",
        default=50,
        min=10,
        max=200
    )
    
    response_timeout: bpy.props.FloatProperty(
        name="Response Timeout",
        description="Timeout for AI responses in seconds",
        default=30.0,
        min=5.0,
        max=120.0
    )
    
    # Privacy settings
    enable_telemetry: bpy.props.BoolProperty(
        name="Enable Telemetry",
        description="Send anonymous usage data to improve the AI",
        default=False
    )
    
    enable_context_sharing: bpy.props.BoolProperty(
        name="Share Scene Context",
        description="Share scene information with AI for better assistance",
        default=True
    )
    
    def draw(self, context):
        """Draw the preferences UI."""
        layout = self.layout
        
        # API Configuration
        api_box = layout.box()
        api_box.label(text="API Configuration", icon='SETTINGS')
        
        api_box.prop(self, "api_key")
        api_box.prop(self, "api_base_url")
        
        # Feature toggles
        features_box = layout.box()
        features_box.label(text="Features", icon='LIGHT')
        
        features_box.prop(self, "enable_ai_features")
        features_box.prop(self, "enable_immersive_ui")
        features_box.prop(self, "enable_workflow_automation")
        
        # Performance settings
        perf_box = layout.box()
        perf_box.label(text="Performance", icon='CONSOLE')
        
        perf_box.prop(self, "max_conversation_history")
        perf_box.prop(self, "response_timeout")
        
        # Privacy settings
        privacy_box = layout.box()
        privacy_box.label(text="Privacy", icon='LOCKED')
        
        privacy_box.prop(self, "enable_telemetry")
        privacy_box.prop(self, "enable_context_sharing")
        
        # Status information
        status_box = layout.box()
        status_box.label(text="Status", icon='INFO')
        
        # Check API connection
        if self.api_key:
            status_box.label(text="✓ API Key configured", icon='CHECKMARK')
        else:
            status_box.label(text="⚠ API Key not configured", icon='ERROR')
        
        # Check features
        if self.enable_ai_features:
            status_box.label(text="✓ AI features enabled", icon='CHECKMARK')
        else:
            status_box.label(text="⚠ AI features disabled", icon='ERROR')


class AthenaMistStartupOperator(bpy.types.Operator):
    """Initialize AthenaMist AI Agent on startup."""
    
    bl_idname = "athena_mist.startup"
    bl_label = "Start AthenaMist"
    bl_description = "Initialize the AthenaMist AI Agent"
    
    def execute(self, context):
        """Execute startup initialization."""
        try:
            # Initialize AthenaMist integration
            preferences = context.preferences.addons[__package__].preferences
            
            if preferences.enable_ai_features:
                # Create global AthenaMist instance
                context.scene.athena_mist = AthenaMistIntegration(
                    api_key=preferences.api_key,
                    base_url=preferences.api_base_url
                )
                
                # Initialize intent parser
                context.scene.intent_parser = IntentParser()
                
                self.report({'INFO'}, "AthenaMist AI Agent initialized successfully!")
            else:
                self.report({'WARNING'}, "AI features are disabled in preferences")
            
            return {'FINISHED'}
        
        except Exception as e:
            self.report({'ERROR'}, f"Failed to initialize AthenaMist: {str(e)}")
            return {'CANCELLED'}


class AthenaMistShutdownOperator(bpy.types.Operator):
    """Shutdown AthenaMist AI Agent."""
    
    bl_idname = "athena_mist.shutdown"
    bl_label = "Stop AthenaMist"
    bl_description = "Shutdown the AthenaMist AI Agent"
    
    def execute(self, context):
        """Execute shutdown cleanup."""
        try:
            # Cleanup AthenaMist instance
            if hasattr(context.scene, 'athena_mist'):
                delattr(context.scene, 'athena_mist')
            
            if hasattr(context.scene, 'intent_parser'):
                delattr(context.scene, 'intent_parser')
            
            self.report({'INFO'}, "AthenaMist AI Agent shutdown successfully!")
            return {'FINISHED'}
        
        except Exception as e:
            self.report({'ERROR'}, f"Failed to shutdown AthenaMist: {str(e)}")
            return {'CANCELLED'}


class AthenaMistTestConnectionOperator(bpy.types.Operator):
    """Test connection to AthenaMist API."""
    
    bl_idname = "athena_mist.test_connection"
    bl_label = "Test Connection"
    bl_description = "Test connection to AthenaMist API"
    
    def execute(self, context):
        """Execute connection test."""
        try:
            preferences = context.preferences.addons[__package__].preferences
            
            if not preferences.api_key:
                self.report({'WARNING'}, "Please configure your API key first")
                return {'CANCELLED'}
            
            # Test connection
            athena = AthenaMistIntegration(
                api_key=preferences.api_key,
                base_url=preferences.api_base_url
            )
            
            # Simple test message
            import asyncio
            response = asyncio.run(athena.send_message("Hello", {}))
            
            if response.confidence > 0.5:
                self.report({'INFO'}, "✓ Connection successful! AthenaMist is ready.")
            else:
                self.report({'WARNING'}, "⚠ Connection test completed with low confidence")
            
            return {'FINISHED'}
        
        except Exception as e:
            self.report({'ERROR'}, f"Connection test failed: {str(e)}")
            return {'CANCELLED'}


class AthenaMistHelpOperator(bpy.types.Operator):
    """Show help and documentation for AthenaMist."""
    
    bl_idname = "athena_mist.help"
    bl_label = "Help"
    bl_description = "Show AthenaMist help and documentation"
    
    def execute(self, context):
        """Execute help display."""
        help_text = """
AthenaMist AI Agent for Blender
==============================

Welcome to AthenaMist, your intelligent AI assistant for 3D creation!

Key Features:
• Natural language understanding for 3D workflows
• Multiple AI personalities (Creative, Technical, Artistic, etc.)
• Immersive UI with beautiful themes
• Workflow automation and optimization
• Real-time scene context awareness

Getting Started:
1. Configure your API key in Preferences
2. Open the AthenaMist panel in the 3D Viewport sidebar
3. Choose your preferred AI mode
4. Start chatting with Athena!

AI Modes:
• Creative Assistant: Inspiring artistic guidance
• Technical Expert: Precise technical assistance
• Artistic Mentor: Artistic development focus
• Workflow Optimizer: Efficiency and performance
• Immersive Storyteller: Narrative and context

For more information, visit:
https://github.com/athenamist/blender-ai-agent
        """
        
        # Show help in a popup
        bpy.ops.wm.call_menu(name="ATHENA_MIST_MT_help")
        
        return {'FINISHED'}


class ATHENA_MIST_MT_help(bpy.types.Menu):
    """Help menu for AthenaMist."""
    
    bl_idname = "ATHENA_MIST_MT_help"
    bl_label = "AthenaMist Help"
    
    def draw(self, context):
        """Draw help menu."""
        layout = self.layout
        
        layout.label(text="AthenaMist AI Agent", icon='BOT')
        layout.separator()
        
        layout.label(text="Your intelligent AI assistant for 3D creation")
        layout.separator()
        
        layout.operator("wm.url_open", text="Documentation", icon='HELP').url = "https://github.com/athenamist/blender-ai-agent"
        layout.operator("wm.url_open", text="Report Issue", icon='URL').url = "https://github.com/athenamist/blender-ai-agent/issues"
        layout.operator("wm.url_open", text="Discord Community", icon='URL').url = "https://discord.gg/athenamist"


# Global variables
_athena_mist_instance = None
_intent_parser_instance = None


def register():
    """Register the AthenaMist AI Agent add-on."""
    # Register preferences
    bpy.utils.register_class(AthenaMistPreferences)
    
    # Register operators
    bpy.utils.register_class(AthenaMistStartupOperator)
    bpy.utils.register_class(AthenaMistShutdownOperator)
    bpy.utils.register_class(AthenaMistTestConnectionOperator)
    bpy.utils.register_class(AthenaMistHelpOperator)
    
    # Register help menu
    bpy.utils.register_class(ATHENA_MIST_MT_help)
    
    # Register UI components
    register_ui()
    
    # Auto-start AthenaMist on add-on enable
    bpy.app.handlers.load_post.append(startup_handler)
    
    print("AthenaMist AI Agent registered successfully!")


def unregister():
    """Unregister the AthenaMist AI Agent add-on."""
    # Remove startup handler
    if startup_handler in bpy.app.handlers.load_post:
        bpy.app.handlers.load_post.remove(startup_handler)
    
    # Unregister UI components
    unregister_ui()
    
    # Unregister help menu
    bpy.utils.unregister_class(ATHENA_MIST_MT_help)
    
    # Unregister operators
    bpy.utils.unregister_class(AthenaMistHelpOperator)
    bpy.utils.unregister_class(AthenaMistTestConnectionOperator)
    bpy.utils.unregister_class(AthenaMistShutdownOperator)
    bpy.utils.unregister_class(AthenaMistStartupOperator)
    
    # Unregister preferences
    bpy.utils.unregister_class(AthenaMistPreferences)
    
    print("AthenaMist AI Agent unregistered successfully!")


def startup_handler(scene):
    """Handler to start AthenaMist on Blender startup."""
    try:
        # Check if add-on is enabled
        if __package__ in bpy.context.preferences.addons:
            # Auto-start AthenaMist
            bpy.ops.athena_mist.startup()
    except Exception as e:
        print(f"AthenaMist startup handler error: {e}")


if __name__ == "__main__":
    register() 