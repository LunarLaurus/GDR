# CI/CD Pipeline

## Topic of Concern
Automated builds for Linux, Windows, and Docker via GitHub Actions

## Requirements
- GitHub Actions workflow: `.github/workflows/build.yml`
- Linux build (ubuntu-latest) → tar.gz artifact
- Windows build (windows-latest) → .exe artifact  
- Docker build → tested image
- Trigger on: push to main, PRs, releases

## Acceptance Criteria
- [x] build.yml exists
- [ ] Linux build job completes successfully
- [ ] Windows build job completes successfully
- [ ] Docker image builds and runs --version
- [ ] Artifacts downloadable after build
