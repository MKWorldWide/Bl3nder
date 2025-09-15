# 👥 Contributing to Bl3nder

We're thrilled you're interested in contributing to Bl3nder! This guide will help you get started with the contribution process.

## 🚀 First Time Contributors

1. **Set Up Your Environment**
   ```bash
   # Fork and clone the repository
   git clone https://github.com/your-username/Bl3nder.git
   cd Bl3nder

   # Install development dependencies
   pip install -r requirements-dev.txt

   # Set up pre-commit hooks
   pre-commit install
   ```

2. **Find an Issue**
   - Check [Good First Issues](https://github.com/MKWorldWide/Bl3nder/contribute)
   - Look for issues tagged with `good first issue` or `help wanted`

3. **Create a Branch**
   ```bash
   git checkout -b feature/amazing-feature
   ```

## 🛠 Development Workflow

### Code Style
- Follow [PEP 8](https://www.python.org/dev/peps/pep-0008/) for Python code
- Use [Google Style](https://google.github.io/styleguide/pyguide.html) docstrings
- Keep lines under 120 characters

### Commit Messages
Follow the [Conventional Commits](https://www.conventionalcommits.org/) specification:

```
<type>(<scope>): <description>

[optional body]

[optional footer(s)]
```

**Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code changes that neither fixes a bug nor adds a feature
- `perf`: Performance improvements
- `test`: Adding tests
- `chore`: Changes to the build process or auxiliary tools

### Testing

#### Run Tests
```bash
# Run all tests
pytest

# Run specific test file
pytest tests/test_module.py

# Run with coverage
pytest --cov=bl3nder tests/
```

### Documentation
- Update relevant documentation
- Add docstrings for new functions/classes
- Update examples if API changes

## 🔄 Pull Request Process

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Open a pull request

### PR Checklist
- [ ] Tests pass
- [ ] Documentation updated
- [ ] Code follows style guidelines
- [ ] Added entry to CHANGELOG.md
- [ ] Issue linked

## 🏗 Project Structure

```
bl3nder/
├── src/                  # Source code
│   ├── core/            # Core functionality
│   ├── ai/              # AI integration
│   └── ui/              # User interface
├── tests/               # Test suite
├── docs/                # Documentation
└── scripts/             # Utility scripts
```

## 💬 Communication

- **Discord**: [Join our community](https://discord.gg/mkworldwide)
- **Issues**: [GitHub Issues](https://github.com/MKWorldWide/Bl3nder/issues)
- **Discussions**: [GitHub Discussions](https://github.com/MKWorldWide/Bl3nder/discussions)

## 🏆 Recognition

All contributors will be recognized in our:
- [Contributors List](CONTRIBUTORS.md)
- Release notes
- Project website

## 📜 Code of Conduct

Please read and follow our [Code of Conduct](code-of-conduct.md).

## 📝 License

By contributing, you agree that your contributions will be licensed under the project's [LICENSE](../LICENSE).

## 🙏 Thank You!

Your contributions make Bl3nder better for everyone. Thank you for being part of our community! 🎉
