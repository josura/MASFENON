# CONTRIBUTION
Thank you for your interest in MASFENON and in contributing to the project

This document provides guidelines to ensure a smooth contribution process and maintain code quality.  

## **Table of Contents**  
- [Getting Started](#getting-started)  
- [Reporting Issues](#reporting-issues)  
- [Contributing Code](#contributing-code)  
- [Coding Style and Standards](#coding-style-and-standards)  
- [Testing and Validation](#testing-and-validation)  
- [Branching and Pull Requests](#branching-and-pull-requests)  
- [Code of Conduct](#code-of-conduct)  

---

## **Getting Started**  

Before contributing, make sure you:  
- Read this document to understand the contribution process.  
- Check the [open issues](https://github.com/josura/MASFENON/issues) to see if your contribution has already been discussed.  
- Fork the repository and create a new branch for your contribution.  

---

## **Reporting Issues**  

If you encounter a bug, have a feature request, or need clarification:  
1. **Search** the [issue tracker](https://github.com/josura/MASFENON/issues) to see if it’s already reported.  
2. **Open a new issue** with a clear title and description.  
3. Provide steps to reproduce the issue if applicable.  
4. Suggest possible fixes or improvements (if possible).  

---

## **Contributing Code**  

### **Prerequisites**  
Ensure you have the following installed:  
- A compatible C++ compiler with MPI support  
- OpenMPI and Armadillo library  
- Any additional dependencies listed in `README.md`  

### **Steps to Contribute**  
1. **Fork** the repository and clone your fork.  
   ```bash
   git clone https://github.com/your-username/MASFENON.git
   cd MASFENON
   ```  
2. **Create a feature branch** for your changes. Have a meaningful name, like the issue that you're addressing or the feature that you are trying to implement. It's best to open an issue before for forking and branching
   ```bash
   git checkout -b feature-branch-name
   ```  
3. Make your changes and **commit with a meaningful message**.  
   ```bash
   git add .
   git commit -m "Add feature X for improved simulation efficiency"
   ```  
4. **Push the branch** to your fork and submit a Pull Request.  
   ```bash
   git push origin feature-branch-name
   ```  

---

## **Coding Style and Standards**  

To maintain consistency, please follow these guidelines:  
- Use **C++17 or newer**.  
- Follow the repository’s established **indentation and formatting** (use `clang-format` if applicable).  
- Write **clear, self-documenting code** with meaningful variable and function names.  
- Include **inline comments** where necessary, especially for complex logic.  

---

## **Testing and Validation**  

Before submitting a PR, ensure your code:  
- Compiles without errors using the specified toolchain.  
- Passes existing tests.  
- Includes **new tests** for new features (if applicable).  

Run tests using:  
```bash
cd build
ctest -C Debug
```

---

## **Branching and Pull Requests**  

- **`main` branch** is stable and protected. All contributions go through PRs.  
- **Feature branches** should be created for each new addition or fix.  
- **Follow PR templates** and link related issues when applicable.  

### **PR Checklist**  
- [ ] Code compiles without errors  
- [ ] Code follows style guidelines  
- [ ] Added/updated relevant tests  
- [ ] Updated documentation if necessary  

---

## **Code of Conduct**  

We follow a respectful and inclusive environment for all contributors. Please read the full [Code of Conduct](CODE_OF_CONDUCT.md) before contributing.  

---

### **Need Help?**  
For questions, discussions, or clarifications, reach out via the [Discussions](https://github.com/josura/MASFENON/discussions) tab or create an issue.  

Happy coding! 🚀  

---

Would you like to add any project-specific details?
