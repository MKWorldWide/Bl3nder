"""
Immersive Interface for Blender AI Agent
=======================================

This module creates an immersive, beautiful UI inspired by creative Skyrim mods
that provides an engaging experience for interacting with the AthenaMist AI agent.
Features include animated elements, dynamic theming, and contextual responses.
"""

import bpy
import bmesh
from bpy.props import StringProperty, EnumProperty, BoolProperty, FloatProperty
from bpy.types import Panel, Operator, PropertyGroup
import json
import asyncio
from typing import Dict, List, Any, Optional
import math
import random
from datetime import datetime

# Import our AI modules
from ..core.athena_mist_integration import AthenaMistIntegration, AthenaMistMode
from ..core.intent_parser import IntentParser


class AthenaMistUIProperties(PropertyGroup):
    """Properties for the AthenaMist UI system."""
    
    # Chat properties
    chat_message: StringProperty(
        name="Message",
        description="Type your message to Athena",
        default="",
        maxlen=1000
    )
    
    # AI Mode selection
    ai_mode: EnumProperty(
        name="AI Mode",
        description="Choose Athena's personality and expertise",
        items=[
            ('CREATIVE_ASSISTANT', "Creative Assistant", "Inspiring and artistic guidance"),
            ('TECHNICAL_EXPERT', "Technical Expert", "Precise technical assistance"),
            ('ARTISTIC_MENTOR', "Artistic Mentor", "Artistic development and techniques"),
            ('WORKFLOW_OPTIMIZER', "Workflow Optimizer", "Efficiency and performance"),
            ('IMMERSIVE_STORYTELLER', "Immersive Storyteller", "Narrative and context")
        ],
        default='CREATIVE_ASSISTANT'
    )
    
    # UI Theme
    ui_theme: EnumProperty(
        name="Theme",
        description="Choose your preferred UI theme",
        items=[
            ('NORDIC', "Nordic", "Inspired by Skyrim's Nordic aesthetic"),
            ('MAGICAL', "Magical", "Mystical and enchanting"),
            ('MODERN', "Modern", "Clean and contemporary"),
            ('VINTAGE', "Vintage", "Classic and timeless"),
            ('CYBERPUNK', "Cyberpunk", "Futuristic and high-tech")
        ],
        default='NORDIC'
    )
    
    # Animation settings
    enable_animations: BoolProperty(
        name="Enable Animations",
        description="Enable smooth UI animations",
        default=True
    )
    
    animation_speed: FloatProperty(
        name="Animation Speed",
        description="Speed of UI animations",
        default=1.0,
        min=0.1,
        max=3.0
    )
    
    # Immersive features
    enable_ambient_effects: BoolProperty(
        name="Ambient Effects",
        description="Enable ambient visual effects",
        default=True
    )
    
    enable_sound_feedback: BoolProperty(
        name="Sound Feedback",
        description="Enable audio feedback for interactions",
        default=False
    )
    
    # Conversation history
    conversation_history: StringProperty(
        name="Conversation History",
        description="JSON string containing conversation history",
        default="[]"
    )
    
    # Current context
    current_context: StringProperty(
        name="Current Context",
        description="JSON string containing current Blender context",
        default="{}"
    )


class AthenaMistChatPanel(Panel):
    """Main chat panel for AthenaMist AI agent."""
    
    bl_label = "AthenaMist AI Assistant"
    bl_idname = "VIEW3D_PT_athena_mist_chat"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = 'AthenaMist'
    bl_options = {'DEFAULT_CLOSED'}
    
    def draw(self, context):
        """Draw the chat panel UI."""
        layout = self.layout
        props = context.scene.athena_mist_ui
        
        # Header with theme-aware styling
        self._draw_header(layout, props)
        
        # Mode selector
        self._draw_mode_selector(layout, props)
        
        # Chat area
        self._draw_chat_area(layout, props)
        
        # Input area
        self._draw_input_area(layout, props)
        
        # Quick actions
        self._draw_quick_actions(layout, props)
        
        # Settings
        self._draw_settings(layout, props)
    
    def _draw_header(self, layout, props):
        """Draw the themed header."""
        box = layout.box()
        row = box.row()
        
        # Theme-aware icon
        icon = self._get_theme_icon(props.ui_theme)
        row.label(text="", icon=icon)
        
        # Title with theme styling
        title_row = row.row()
        title_row.label(text="AthenaMist AI", icon='BOT')
        
        # Status indicator
        status_row = row.row()
        status_row.alignment = 'RIGHT'
        status_row.label(text="●", icon='OUTLINER_OB_LIGHT')
    
    def _draw_mode_selector(self, layout, props):
        """Draw the AI mode selector."""
        box = layout.box()
        box.label(text="Athena's Mode", icon='SETTINGS')
        
        row = box.row()
        row.prop(props, "ai_mode", text="")
        
        # Mode description
        mode_desc = self._get_mode_description(props.ai_mode)
        desc_row = box.row()
        desc_row.label(text=mode_desc, icon='INFO')
    
    def _draw_chat_area(self, layout, props):
        """Draw the chat conversation area."""
        box = layout.box()
        box.label(text="Conversation", icon='COMMENT')
        
        # Chat messages area
        chat_area = box.column()
        chat_area.scale_y = 0.8
        
        # Load conversation history
        try:
            history = json.loads(props.conversation_history)
            for message in history[-5:]:  # Show last 5 messages
                self._draw_message(chat_area, message)
        except json.JSONDecodeError:
            chat_area.label(text="Start a conversation with Athena!", icon='BOT')
    
    def _draw_message(self, layout, message):
        """Draw a single chat message."""
        msg_type = message.get('type', 'user')
        
        if msg_type == 'user':
            row = layout.row()
            row.alignment = 'RIGHT'
            row.label(text=message.get('content', ''), icon='USER')
        else:
            row = layout.row()
            row.alignment = 'LEFT'
            row.label(text=message.get('content', ''), icon='BOT')
    
    def _draw_input_area(self, layout, props):
        """Draw the message input area."""
        box = layout.box()
        
        # Message input
        row = box.row()
        row.prop(props, "chat_message", text="")
        
        # Send button
        send_row = box.row()
        send_row.operator("athena_mist.send_message", text="Send", icon='PLAY')
        
        # Voice input button (future feature)
        voice_row = box.row()
        voice_row.operator("athena_mist.voice_input", text="Voice", icon='SPEAKER')
    
    def _draw_quick_actions(self, layout, props):
        """Draw quick action buttons."""
        box = layout.box()
        box.label(text="Quick Actions", icon='LIGHT')
        
        # Quick action buttons
        actions_row = box.row()
        actions_row.operator("athena_mist.quick_create", text="Create", icon='ADD')
        actions_row.operator("athena_mist.quick_material", text="Material", icon='MATERIAL')
        actions_row.operator("athena_mist.quick_animate", text="Animate", icon='ANIM')
    
    def _draw_settings(self, layout, props):
        """Draw settings area."""
        box = layout.box()
        box.label(text="Settings", icon='PREFERENCES')
        
        # Theme selector
        theme_row = box.row()
        theme_row.prop(props, "ui_theme", text="Theme")
        
        # Animation settings
        anim_row = box.row()
        anim_row.prop(props, "enable_animations", text="Animations")
        
        if props.enable_animations:
            speed_row = box.row()
            speed_row.prop(props, "animation_speed", text="Speed")
        
        # Immersive features
        immersive_row = box.row()
        immersive_row.prop(props, "enable_ambient_effects", text="Ambient Effects")
        
        sound_row = box.row()
        sound_row.prop(props, "enable_sound_feedback", text="Sound Feedback")
    
    def _get_theme_icon(self, theme):
        """Get theme-appropriate icon."""
        icons = {
            'NORDIC': 'GHOST_ENABLED',
            'MAGICAL': 'LIGHT',
            'MODERN': 'BLENDER',
            'VINTAGE': 'TIME',
            'CYBERPUNK': 'CONSOLE'
        }
        return icons.get(theme, 'BLENDER')
    
    def _get_mode_description(self, mode):
        """Get description for AI mode."""
        descriptions = {
            'CREATIVE_ASSISTANT': "Inspiring creative guidance",
            'TECHNICAL_EXPERT': "Precise technical assistance",
            'ARTISTIC_MENTOR': "Artistic development focus",
            'WORKFLOW_OPTIMIZER': "Efficiency and performance",
            'IMMERSIVE_STORYTELLER': "Narrative and context"
        }
        return descriptions.get(mode, "Creative assistance")


class AthenaMistSendMessageOperator(Operator):
    """Send message to AthenaMist AI."""
    
    bl_idname = "athena_mist.send_message"
    bl_label = "Send Message"
    bl_description = "Send message to AthenaMist AI assistant"
    
    def execute(self, context):
        """Execute the send message operation."""
        props = context.scene.athena_mist_ui
        
        if not props.chat_message.strip():
            self.report({'WARNING'}, "Please enter a message")
            return {'CANCELLED'}
        
        # Get current Blender context
        blender_context = self._get_blender_context(context)
        
        # Send message to AthenaMist
        self._send_message_async(props.chat_message, blender_context, props.ai_mode)
        
        # Clear input
        props.chat_message = ""
        
        return {'FINISHED'}
    
    def _get_blender_context(self, context):
        """Get current Blender scene context."""
        scene = context.scene
        
        # Collect objects
        objects = []
        for obj in scene.objects:
            objects.append({
                'name': obj.name,
                'type': obj.type,
                'location': [obj.location.x, obj.location.y, obj.location.z]
            })
        
        # Collect materials
        materials = []
        for mat in bpy.data.materials:
            materials.append({
                'name': mat.name,
                'use_nodes': mat.use_nodes
            })
        
        # Render settings
        render_settings = {
            'engine': scene.render.engine,
            'resolution_x': scene.render.resolution_x,
            'resolution_y': scene.render.resolution_y
        }
        
        return {
            'objects': objects,
            'materials': materials,
            'render_settings': render_settings,
            'user_mode': context.mode
        }
    
    def _send_message_async(self, message, context, mode):
        """Send message to AthenaMist asynchronously."""
        # This would be implemented with proper async handling
        # For now, we'll simulate the response
        
        # Create AthenaMist integration
        athena = AthenaMistIntegration()
        
        # Convert mode string to enum
        mode_enum = AthenaMistMode(mode)
        
        # Send message (this would be async in real implementation)
        response = asyncio.run(athena.send_message(message, context, mode_enum))
        
        # Update conversation history
        self._update_conversation_history(message, response.content)
        
        # Execute any suggested actions
        self._execute_suggested_actions(response.actions)
    
    def _update_conversation_history(self, user_message, ai_response):
        """Update conversation history."""
        # This would update the conversation history in the UI properties
        pass
    
    def _execute_suggested_actions(self, actions):
        """Execute suggested actions from AI response."""
        for action in actions:
            action_type = action.get('type')
            if action_type == 'create_object':
                self._create_object(action)
            elif action_type == 'modify_material':
                self._modify_material(action)
            elif action_type == 'setup_animation':
                self._setup_animation(action)
    
    def _create_object(self, action):
        """Create object based on AI suggestion."""
        object_type = action.get('object_type', 'MESH')
        
        if object_type == 'MESH':
            bpy.ops.mesh.primitive_cube_add()
        elif object_type == 'LIGHT':
            bpy.ops.object.light_add()
        elif object_type == 'CAMERA':
            bpy.ops.object.camera_add()
    
    def _modify_material(self, action):
        """Modify material based on AI suggestion."""
        # Implementation for material modification
        pass
    
    def _setup_animation(self, action):
        """Setup animation based on AI suggestion."""
        # Implementation for animation setup
        pass


class AthenaMistQuickCreateOperator(Operator):
    """Quick create objects with AI assistance."""
    
    bl_idname = "athena_mist.quick_create"
    bl_label = "Quick Create"
    bl_description = "Quickly create objects with AI assistance"
    
    def execute(self, context):
        """Execute quick create operation."""
        # Show quick create menu
        bpy.ops.wm.call_menu(name="ATHENA_MIST_MT_quick_create")
        return {'FINISHED'}


class AthenaMistQuickMaterialOperator(Operator):
    """Quick material creation with AI assistance."""
    
    bl_idname = "athena_mist.quick_material"
    bl_label = "Quick Material"
    bl_description = "Quickly create materials with AI assistance"
    
    def execute(self, context):
        """Execute quick material operation."""
        # Show quick material menu
        bpy.ops.wm.call_menu(name="ATHENA_MIST_MT_quick_material")
        return {'FINISHED'}


class AthenaMistQuickAnimateOperator(Operator):
    """Quick animation setup with AI assistance."""
    
    bl_idname = "athena_mist.quick_animate"
    bl_label = "Quick Animate"
    bl_description = "Quickly setup animations with AI assistance"
    
    def execute(self, context):
        """Execute quick animate operation."""
        # Show quick animate menu
        bpy.ops.wm.call_menu(name="ATHENA_MIST_MT_quick_animate")
        return {'FINISHED'}


class AthenaMistVoiceInputOperator(Operator):
    """Voice input for AthenaMist (future feature)."""
    
    bl_idname = "athena_mist.voice_input"
    bl_label = "Voice Input"
    bl_description = "Use voice input for AthenaMist (coming soon)"
    
    def execute(self, context):
        """Execute voice input operation."""
        self.report({'INFO'}, "Voice input feature coming soon!")
        return {'FINISHED'}


# Menu classes for quick actions
class ATHENA_MIST_MT_quick_create(bpy.types.Menu):
    """Quick create menu."""
    
    bl_idname = "ATHENA_MIST_MT_quick_create"
    bl_label = "Quick Create"
    
    def draw(self, context):
        """Draw quick create menu."""
        layout = self.layout
        
        layout.operator("mesh.primitive_cube_add", text="Cube", icon='MESH_CUBE')
        layout.operator("mesh.primitive_uv_sphere_add", text="Sphere", icon='MESH_UVSPHERE')
        layout.operator("mesh.primitive_cylinder_add", text="Cylinder", icon='MESH_CYLINDER')
        layout.operator("mesh.primitive_plane_add", text="Plane", icon='MESH_PLANE')
        
        layout.separator()
        
        layout.operator("object.light_add", text="Light", icon='LIGHT')
        layout.operator("object.camera_add", text="Camera", icon='CAMERA_DATA')


class ATHENA_MIST_MT_quick_material(bpy.types.Menu):
    """Quick material menu."""
    
    bl_idname = "ATHENA_MIST_MT_quick_material"
    bl_label = "Quick Material"
    
    def draw(self, context):
        """Draw quick material menu."""
        layout = self.layout
        
        layout.operator("material.new", text="New Material", icon='MATERIAL')
        layout.operator("material.new", text="Metal", icon='MATERIAL').name = "Metal"
        layout.operator("material.new", text="Glass", icon='MATERIAL').name = "Glass"
        layout.operator("material.new", text="Wood", icon='MATERIAL').name = "Wood"


class ATHENA_MIST_MT_quick_animate(bpy.types.Menu):
    """Quick animate menu."""
    
    bl_idname = "ATHENA_MIST_MT_quick_animate"
    bl_label = "Quick Animate"
    
    def draw(self, context):
        """Draw quick animate menu."""
        layout = self.layout
        
        layout.operator("anim.keyframe_insert", text="Insert Keyframe", icon='KEYINGSET')
        layout.operator("anim.keyframe_insert_menu", text="Insert Keyframe Menu", icon='KEYINGSET')
        layout.operator("anim.keyframe_delete", text="Delete Keyframe", icon='KEYINGSET')


# Registration
classes = [
    AthenaMistUIProperties,
    AthenaMistChatPanel,
    AthenaMistSendMessageOperator,
    AthenaMistQuickCreateOperator,
    AthenaMistQuickMaterialOperator,
    AthenaMistQuickAnimateOperator,
    AthenaMistVoiceInputOperator,
    ATHENA_MIST_MT_quick_create,
    ATHENA_MIST_MT_quick_material,
    ATHENA_MIST_MT_quick_animate
]


def register():
    """Register the AthenaMist UI system."""
    for cls in classes:
        bpy.utils.register_class(cls)
    
    # Register properties
    bpy.types.Scene.athena_mist_ui = bpy.props.PointerProperty(type=AthenaMistUIProperties)


def unregister():
    """Unregister the AthenaMist UI system."""
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)
    
    # Unregister properties
    del bpy.types.Scene.athena_mist_ui


if __name__ == "__main__":
    register() 