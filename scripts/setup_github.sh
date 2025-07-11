#!/bin/bash

# Bl3nder GitHub Setup Script
# This script helps set up the initial GitHub repository and push the project

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check git status
check_git_status() {
    if ! command_exists git; then
        print_error "Git is not installed. Please install Git first."
        exit 1
    fi
    
    if [ ! -d ".git" ]; then
        print_error "This is not a Git repository. Please initialize Git first."
        exit 1
    fi
}

# Function to check GitHub CLI
check_github_cli() {
    if ! command_exists gh; then
        print_warning "GitHub CLI is not installed. Some features may not work."
        print_warning "Install GitHub CLI: https://cli.github.com/"
        return 1
    fi
    
    if ! gh auth status >/dev/null 2>&1; then
        print_warning "GitHub CLI is not authenticated. Please run 'gh auth login' first."
        return 1
    fi
    
    return 0
}

# Function to create GitHub repository
create_github_repo() {
    local repo_name="bl3nder"
    local description="AI-Enhanced Blender Fork with Advanced Rendering and Workflow Automation"
    local visibility="public"
    local homepage="https://github.com/your-org/bl3nder"
    
    print_status "Creating GitHub repository..."
    
    if gh repo create "$repo_name" \
        --description "$description" \
        --public \
        --homepage "$homepage" \
        --source . \
        --remote origin \
        --push; then
        print_success "GitHub repository created successfully!"
        return 0
    else
        print_error "Failed to create GitHub repository."
        return 1
    fi
}

# Function to set up repository settings
setup_repo_settings() {
    print_status "Setting up repository settings..."
    
    # Enable features
    gh repo edit --enable-projects --enable-discussions --enable-wiki=false
    
    # Set topics
    gh repo edit --add-topic "3d,ai,blender,rendering,computer-graphics,machine-learning,open-source,python,c-plus-plus,real-time,progressive-rendering,workflow-automation,scene-analysis,performance-optimization"
    
    print_success "Repository settings configured!"
}

# Function to create initial release
create_initial_release() {
    local version="v1.0.0"
    local tag_name="$version"
    local release_title="Bl3nder $version - Initial Public Release"
    local release_body="## 🚀 Bl3nder $version - Initial Public Release

### 🎉 What's New
- **AI-Enhanced 3D Creation**: Advanced AI integration with Blender
- **Lilybear Render Engine**: Next-generation render engine with real-time capabilities
- **AthenaMist AI Agent**: Comprehensive AI assistant for 3D workflows
- **Community-Driven Development**: Full public repository with community collaboration
- **Cross-Platform Support**: Windows, macOS, and Linux support

### 🤖 AI Features
- Scene complexity analysis and optimization
- Real-time performance monitoring and adaptive quality
- Workflow automation and creative assistance
- AI-powered rendering enhancement

### 🎨 Render Engines
- **Cycles**: Path-tracing renderer for photorealistic images
- **EEVEE**: Real-time renderer for interactive preview
- **Workbench**: Fast viewport rendering
- **Hydra**: USD-based rendering system
- **Lilybear**: AI-enhanced render engine with real-time capabilities

### 📦 Downloads
- **macOS**: bl3nder-macos-$version.dmg
- **Windows**: bl3nder-windows-$version.exe
- **Linux**: bl3nder-linux-x64-$version.tar.gz

### 🔧 Installation
1. Download the appropriate file for your platform
2. Extract/install the package
3. Launch Bl3nder and enable AI features
4. Start creating with AI-enhanced tools!

### 📚 Documentation
- [User Guide](docs/)
- [API Documentation](doc/python_api/)
- [Community Guide](docs/COMMUNITY.md)
- [Contributing Guide](CONTRIBUTING.md)

### 🤝 Community
- [GitHub Discussions](https://github.com/your-org/bl3nder/discussions)
- [Issue Tracker](https://github.com/your-org/bl3nder/issues)
- [Contributing](CONTRIBUTING.md)

### 🙏 Acknowledgments
Thank you to all contributors, the Blender Foundation, and the open source community!

---

**Welcome to the future of AI-enhanced 3D creation!** 🚀

*Bl3nder - Where AI meets 3D creativity*"
    
    print_status "Creating initial release..."
    
    # Create tag
    git tag -a "$tag_name" -m "Release $version"
    git push origin "$tag_name"
    
    # Create release
    if gh release create "$tag_name" \
        --title "$release_title" \
        --notes "$release_body" \
        --draft=false \
        --prerelease=false; then
        print_success "Initial release created successfully!"
        return 0
    else
        print_error "Failed to create initial release."
        return 1
    fi
}

# Function to set up branch protection
setup_branch_protection() {
    print_status "Setting up branch protection..."
    
    # Protect main branch
    gh api repos/:owner/:repo/branches/main/protection \
        --method PUT \
        --field required_status_checks='{"strict":true,"contexts":["Build and Test (Linux)","Build and Test (macOS)","Build and Test (Windows)","Code Quality","Documentation Check"]}' \
        --field enforce_admins=true \
        --field required_pull_request_reviews='{"required_approving_review_count":2,"dismiss_stale_reviews":true,"require_code_owner_reviews":true}' \
        --field restrictions='{"users":[],"teams":["bl3nder-maintainers"]}'
    
    print_success "Branch protection configured!"
}

# Function to create teams
create_teams() {
    print_status "Creating teams..."
    
    # Create maintainers team
    gh api orgs/:org/teams \
        --method POST \
        --field name="bl3nder-maintainers" \
        --field description="Bl3nder project maintainers" \
        --field privacy="closed"
    
    # Create AI team
    gh api orgs/:org/teams \
        --method POST \
        --field name="ai-team" \
        --field description="AI features and AthenaMist integration team" \
        --field privacy="closed"
    
    # Create rendering team
    gh api orgs/:org/teams \
        --method POST \
        --field name="rendering-team" \
        --field description="Render engine development team" \
        --field privacy="closed"
    
    # Create documentation team
    gh api orgs/:org/teams \
        --method POST \
        --field name="documentation-team" \
        --field description="Documentation and user guides team" \
        --field privacy="closed"
    
    # Create automation team
    gh api orgs/:org/teams \
        --method POST \
        --field name="automation-team" \
        --field description="CI/CD and automation team" \
        --field privacy="closed"
    
    # Create testing team
    gh api orgs/:org/teams \
        --method POST \
        --field name="testing-team" \
        --field description="Testing and quality assurance team" \
        --field privacy="closed"
    
    print_success "Teams created successfully!"
}

# Function to set up projects
setup_projects() {
    print_status "Setting up projects..."
    
    # Create main development project
    gh api repos/:owner/:repo/projects \
        --method POST \
        --field name="Bl3nder Development" \
        --field description="Main development project for Bl3nder" \
        --field visibility="public"
    
    # Create AI features project
    gh api repos/:owner/:repo/projects \
        --method POST \
        --field name="AI Features" \
        --field description="AI-related features and improvements" \
        --field visibility="public"
    
    # Create render engines project
    gh api repos/:owner/:repo/projects \
        --method POST \
        --field name="Render Engines" \
        --field description="Render engine development and improvements" \
        --field visibility="public"
    
    print_success "Projects created successfully!"
}

# Function to enable features
enable_features() {
    print_status "Enabling repository features..."
    
    # Enable security features
    gh api repos/:owner/:repo/vulnerability-alerts \
        --method PUT
    
    gh api repos/:owner/:repo/automated-security-fixes \
        --method PUT
    
    # Enable dependency graph
    gh api repos/:owner/:repo/dependency-graph \
        --method PUT
    
    print_success "Repository features enabled!"
}

# Function to push all changes
push_changes() {
    print_status "Pushing all changes to GitHub..."
    
    # Add all files
    git add .
    
    # Commit changes
    git commit -m "Initial public release of Bl3nder

- AI-enhanced Blender fork with advanced features
- Lilybear render engine with real-time capabilities
- AthenaMist AI agent for workflow automation
- Comprehensive documentation and community tools
- Optimized CI/CD pipeline for quality assurance
- Cross-platform support (Windows, macOS, Linux)

This release represents a major milestone in AI-enhanced 3D creation,
bringing together cutting-edge AI technology with the world's most
powerful open-source 3D creation suite."
    
    # Push to main branch
    git push origin main
    
    print_success "All changes pushed to GitHub!"
}

# Function to display next steps
show_next_steps() {
    echo
    print_success "GitHub setup completed successfully!"
    echo
    echo "🎉 Next steps:"
    echo "1. Review the repository at: https://github.com/your-org/bl3nder"
    echo "2. Check the initial release: https://github.com/your-org/bl3nder/releases"
    echo "3. Set up team permissions and access"
    echo "4. Configure branch protection rules"
    echo "5. Set up project boards and milestones"
    echo "6. Enable GitHub Pages for documentation"
    echo "7. Configure repository secrets for CI/CD"
    echo "8. Set up community guidelines and moderation"
    echo
    echo "📚 Documentation:"
    echo "- [CI/CD Guide](docs/CI_CD.md)"
    echo "- [Community Guide](docs/COMMUNITY.md)"
    echo "- [Contributing Guide](CONTRIBUTING.md)"
    echo "- [Public Release Guide](docs/PUBLIC_RELEASE.md)"
    echo
    echo "🤝 Community:"
    echo "- [GitHub Discussions](https://github.com/your-org/bl3nder/discussions)"
    echo "- [Issue Tracker](https://github.com/your-org/bl3nder/issues)"
    echo "- [Contributing](CONTRIBUTING.md)"
    echo
    echo "🚀 Welcome to the future of AI-enhanced 3D creation!"
}

# Main execution
main() {
    echo "🚀 Bl3nder GitHub Setup Script"
    echo "================================"
    echo
    
    # Check prerequisites
    check_git_status
    check_github_cli
    
    # Ask for confirmation
    echo "This script will:"
    echo "1. Create a new GitHub repository"
    echo "2. Set up repository settings and features"
    echo "3. Create teams and projects"
    echo "4. Set up branch protection"
    echo "5. Create an initial release"
    echo "6. Push all changes to GitHub"
    echo
    read -p "Do you want to continue? (y/N): " -n 1 -r
    echo
    
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        print_warning "Setup cancelled."
        exit 0
    fi
    
    # Execute setup steps
    push_changes
    create_github_repo
    setup_repo_settings
    create_teams
    setup_projects
    setup_branch_protection
    enable_features
    create_initial_release
    
    # Show next steps
    show_next_steps
}

# Run main function
main "$@" 