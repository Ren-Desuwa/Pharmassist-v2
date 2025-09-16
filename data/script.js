// Dashboard Application State Management
class PharmAssistApp {
    constructor() {
        this.currentUser = null;
        this.prescriptions = [];
        this.notifications = [];
        this.patients = [];
        this.currentPage = 'prescriptionOrder';
        this.isMobileMenuOpen = false;
        this.initializeUser();
        this.initializeEventListeners();
        this.fetchAllData();
    }

    // Logging utility
    logEvent(context, details) {
        const timestamp = new Date().toISOString();
        const logMsg = `[${timestamp}] ${context} : ${JSON.stringify(details)}`;
        console.log(logMsg);
        // Send to backend
        fetch('/api/log', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ context, details })
        }).catch(() => {});
    }

    // Initialize user from session storage and validate session
    async initializeUser() {
        const userData = sessionStorage.getItem('pharmassist-user');
        this.logEvent('index', { session: 'initializing' });
        if (userData) {
            this.currentUser = JSON.parse(userData);
            this.logEvent('index', { session: 'verifying', user: this.currentUser.name });
            // Validate session with backend
            try {
                const res = await fetch('/api/validate-session', { credentials: 'include' });
                const result = await res.json();
                this.logEvent('index', { session: 'validate-session response', response: result });
                if (!result.valid) {
                    this.logEvent('index', { session: 'verifying', result: 'failed' });
                    sessionStorage.removeItem('pharmassist-user');
                    // window.location.href = 'login.html';
                    return;
                } else {
                    this.logEvent('index', { session: 'verifying', result: 'success', session_token: this.getSessionTokenFromCookie(), user: result.username || this.currentUser.name });
                }
            } catch (e) {
                this.logEvent('index', { session: 'verifying', result: 'failed', error: e.toString() });
                // window.location.href = 'login.html';
                return;
            }
            this.updateUserInterface();
        } else {
            this.logEvent('index', { session: 'no user in sessionStorage' });
            // window.location.href = 'login.html';
        }
    }
    // Helper to get session token from cookie
    getSessionTokenFromCookie() {
        const match = document.cookie.match(/session_token=([^;]+)/);
        return match ? match[1] : null;
    }

    // Update user interface with current user data
    updateUserInterface() {
        const elements = ['physicianName', 'sidebarPhysicianName'];
        elements.forEach(id => {
            const element = document.getElementById(id);
            if (element) {
                element.textContent = this.currentUser.name;
            }
        });
    }

    async fetchAllData() {
        await Promise.all([
            this.fetchPrescriptions(),
            this.fetchNotifications(),
            this.fetchPatients()
        ]);
        this.updateNotificationBadges();
        this.loadInitialData();
    }

    async fetchPrescriptions() {
        try {
            const res = await fetch('/api/prescriptions', { credentials: 'include' });
            const result = await res.json();
            if (result.success && Array.isArray(result.data)) {
                this.prescriptions = result.data;
            } else {
                this.prescriptions = [];
            }
        } catch {
            this.prescriptions = [];
        }
    }

    async fetchNotifications() {
        try {
            const res = await fetch('/api/notifications', { credentials: 'include' });
            const result = await res.json();
            if (result.success && Array.isArray(result.data)) {
                this.notifications = result.data;
            } else {
                this.notifications = [];
            }
        } catch {
            this.notifications = [];
        }
    }

    async fetchPatients() {
        try {
            const res = await fetch('/api/patients', { credentials: 'include' });
            const result = await res.json();
            if (result.success && Array.isArray(result.data)) {
                this.patients = result.data;
            } else {
                this.patients = [];
            }
        } catch {
            this.patients = [];
        }
    }

    // Initialize event listeners
    initializeEventListeners() {
        // Prescription form
        const prescriptionForm = document.getElementById('prescriptionForm');
        if (prescriptionForm) {
            prescriptionForm.addEventListener('submit', (e) => this.handlePrescriptionSubmit(e));
        }

        // Autocomplete for patient name
        const patientNameInput = document.getElementById('patientName');
        const patientMRNInput = document.getElementById('patientMRN');
        if (patientNameInput) {
            patientNameInput.addEventListener('input', (e) => this.showPatientAutocomplete('name', e.target.value));
            patientNameInput.addEventListener('focus', (e) => this.showPatientAutocomplete('name', e.target.value));
            patientNameInput.addEventListener('blur', () => setTimeout(() => this.hideAutocomplete('patientName-autocomplete'), 200));
        }
        if (patientMRNInput) {
            patientMRNInput.addEventListener('input', (e) => this.showPatientAutocomplete('mrn', e.target.value));
            patientMRNInput.addEventListener('focus', (e) => this.showPatientAutocomplete('mrn', e.target.value));
            patientMRNInput.addEventListener('blur', () => setTimeout(() => this.hideAutocomplete('patientMRN-autocomplete'), 200));
        }

        // Initialize theme
        const savedTheme = localStorage.getItem('pharmassist-theme') || 'dark';
        document.body.setAttribute('data-theme', savedTheme);
    }

    // Logout method
    logout() {
        // Clear session data
        sessionStorage.removeItem('pharmassist-user');
        
        // Close mobile menu if open
        if (this.isMobileMenuOpen) {
            this.toggleMobileMenu();
        }
        
        // Redirect to login page
        window.location.href = 'login.html'; // Adjust path as needed
    }

    // Navigation methods
    showPage(pageId) {
        this.logEvent('index', { session: this.currentUser ? this.currentUser.name : null, action: `open ${pageId}` });
        // Hide all pages
        document.querySelectorAll('.page').forEach(page => {
            page.classList.add('hidden');
        });
        
        // Remove active class from all nav links
        document.querySelectorAll('.nav-link').forEach(link => {
            link.classList.remove('active');
        });
        
        // Show selected page
        const targetPage = document.getElementById(pageId);
        if (targetPage) {
            targetPage.classList.remove('hidden');
        }
        
        // Add active class to clicked nav link
        const activeLink = document.querySelector(`[onclick="showPage('${pageId}')"]`);
        if (activeLink) {
            activeLink.classList.add('active');
        }

        // Load page-specific data
        this.loadPageData(pageId);
        this.currentPage = pageId;

        // Close mobile menu after navigation
        if (this.isMobileMenuOpen) {
            this.toggleMobileMenu();
        }
    }

    loadPageData(pageId) {
        switch(pageId) {
            case 'activePrescriptions':
                this.loadActivePrescriptions();
                break;
            case 'prescriptionHistory':
                this.loadPrescriptionHistory();
                break;
            case 'notifications':
                this.loadNotifications();
                break;
        }
    }

    loadInitialData() {
        this.loadActivePrescriptions();
        this.loadNotifications();
        this.loadPrescriptionHistory();
    }

    // Mobile menu methods
    toggleMobileMenu() {
        const sidebar = document.getElementById('sidebar');
        const overlay = document.getElementById('mobileOverlay');
        
        this.isMobileMenuOpen = !this.isMobileMenuOpen;
        
        if (this.isMobileMenuOpen) {
            sidebar.classList.add('mobile-open');
            overlay.classList.add('active');
            document.body.style.overflow = 'hidden';
        } else {
            sidebar.classList.remove('mobile-open');
            overlay.classList.remove('active');
            document.body.style.overflow = '';
        }
    }

    // Prescription management methods
    async handlePrescriptionSubmit(e) {
        this.logEvent('index', { session: this.currentUser ? this.currentUser.name : null, action: 'submit prescription' });
        e.preventDefault();
        const form = e.target;
        // Collect all medication fields
        const medicationBlocks = form.querySelectorAll('.medication-fields');
        const medications = Array.from(medicationBlocks).map((block, idx) => ({
            medicationName: block.querySelector('[name="medicationName"]').value,
            strength: block.querySelector('[name="strength"]').value,
            dosageForm: block.querySelector('[name="dosageForm"]').value,
            quantity: parseInt(block.querySelector('[name="quantity"]').value)
        }));
        const prescriptionData = {
            id: this.generatePrescriptionId(),
            patientName: document.getElementById('patientName').value,
            patientMRN: document.getElementById('patientMRN').value,
            ward: document.getElementById('patientWard').value,
            bedNumber: document.getElementById('bedNumber').value,
            medications: medications,
            route: document.getElementById('route').value,
            frequency: document.getElementById('frequency').value,
            priority: document.getElementById('priority').value,
            indication: document.getElementById('indication').value,
            specialInstructions: document.getElementById('specialInstructions').value,
            status: 'pending',
            date: new Date().toISOString().split('T')[0],
            prescribingPhysician: `Dr. ${this.currentUser ? this.currentUser.name : ''}`
        };
        // Send to backend
        try {
            const res = await fetch('/api/prescription', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(prescriptionData)
            });
            const result = await res.json();
            if (result.success) {
                this.showNotification('Prescription submitted successfully!', 'success');
                form.reset();
                await this.fetchPrescriptions();
                this.loadActivePrescriptions();
            } else {
                this.showNotification(result.message || 'Submission failed.', 'error');
            }
        } catch {
            this.showNotification('Submission failed. Check connection.', 'error');
        }
    }

    generatePrescriptionId() {
        const year = new Date().getFullYear();
        const orderNumber = String(this.prescriptions.length + 1).padStart(3, '0');
        return `RX-${year}-${orderNumber}`;
    }

    // Data loading methods
    loadActivePrescriptions() {
        const activePrescriptions = this.prescriptions.filter(rx => 
            ['pending', 'processing', 'dispensing', 'ready'].includes(rx.status)
        );

        const container = document.getElementById('activeOrdersList');
        if (!container) return;

        if (activePrescriptions.length === 0) {
            container.innerHTML = this.createEmptyState('📋', 'No Active Prescriptions', 'All prescription orders have been completed or there are no pending orders.');
            return;
        }

        container.innerHTML = activePrescriptions.map(rx => this.createPrescriptionCard(rx, true)).join('');
    }

    loadPrescriptionHistory() {
        const completedPrescriptions = this.prescriptions.filter(rx => 
            ['dispensed', 'partially-dispensed', 'cancelled'].includes(rx.status)
        );

        const container = document.getElementById('historyOrdersList');
        if (!container) return;

        if (completedPrescriptions.length === 0) {
            container.innerHTML = this.createEmptyState('📚', 'No Prescription History', 'Completed prescriptions will appear here once they are dispensed.');
            return;
        }

        container.innerHTML = completedPrescriptions.map(rx => this.createPrescriptionCard(rx, false)).join('');
    }

    loadNotifications() {
        this.logEvent('index', { session: this.currentUser ? this.currentUser.name : null, action: 'open notif' });
        const container = document.getElementById('notificationsList');
        if (!container) return;

        if (this.notifications.length === 0) {
            container.innerHTML = this.createEmptyState('🔔', 'No Notifications', 'Pharmacy updates and medication alerts will appear here.');
            return;
        }

        container.innerHTML = this.notifications.map(notification => this.createNotificationCard(notification)).join('');
    }

    // UI Creation methods
    createPrescriptionCard(prescription, isActive = true) {
        const priorityClass = `priority-${prescription.priority}`;
        const statusClass = `status-${prescription.status}`;
        
        const actionButtons = isActive ? this.createActiveOrderActions(prescription) : '';
        const completionInfo = !isActive ? this.createCompletionInfo(prescription) : '';

        return `
            <div class="order-item">
                <div class="order-info">
                    <h4>${prescription.medicationName} ${prescription.strength} (${prescription.dosageForm})</h4>
                    <div class="order-meta">
                        <div class="meta-row"><strong>Patient:</strong> ${prescription.patientName} (${prescription.patientMRN})</div>
                        <div class="meta-row"><strong>Quantity:</strong> ${prescription.quantity} units | <strong>Route:</strong> ${prescription.route} | <strong>Frequency:</strong> ${prescription.frequency}</div>
                        <div class="meta-row"><strong>Ward:</strong> ${this.formatWardName(prescription.ward)} ${prescription.bedNumber ? `| Bed: ${prescription.bedNumber}` : ''}</div>
                        <div class="meta-row"><strong>Indication:</strong> ${prescription.indication || 'Not specified'}</div>
                        <div class="meta-row"><strong>Order Date:</strong> ${this.formatDate(prescription.date)} | <strong>ID:</strong> ${prescription.id}</div>
                        ${completionInfo}
                    </div>
                </div>
                <div class="order-actions">
                    <span class="status-badge ${statusClass}">${this.formatStatus(prescription.status)}</span>
                    <span class="priority-indicator ${priorityClass}">${prescription.priority.toUpperCase()}</span>
                    ${actionButtons}
                </div>
            </div>
        `;
    }

    createActiveOrderActions(prescription) {
        let actions = '';
        
        if (prescription.status === 'ready') {
            actions += `<button class="btn btn-sm btn-success" onclick="app.collectMedication('${prescription.id}')">Collect</button>`;
        }
        
        if (['pending', 'processing'].includes(prescription.status)) {
            actions += `<button class="btn btn-sm btn-warning" onclick="app.modifyPrescription('${prescription.id}')">Modify</button>`;
            actions += `<button class="btn btn-sm btn-danger" onclick="app.cancelPrescription('${prescription.id}')">Cancel</button>`;
        }

        return actions;
    }

    createCompletionInfo(prescription) {
        let info = '';
        
        if (prescription.status === 'dispensed') {
            info = `<div class="meta-row"><strong>Dispensed:</strong> ${this.formatDate(prescription.completedDate)} by ${prescription.dispensedBy}</div>`;
        } else if (prescription.status === 'partially-dispensed') {
            info = `<div class="meta-row"><strong>Partially Dispensed:</strong> ${prescription.partialQuantity}/${prescription.quantity} units</div>`;
            info += `<div class="meta-row"><strong>Reason:</strong> ${prescription.partialReason}</div>`;
        }

        return info;
    }

    createNotificationCard(notification) {
        const notificationClass = notification.read ? '' : 'unread';
        const typeClass = notification.type === 'urgent' ? 'urgent' : notification.type === 'success' ? 'success' : '';
        
        return `
            <div class="notification-item ${notificationClass} ${typeClass}">
                <div class="notification-header">
                    <div class="notification-title">${notification.title}</div>
                    <div class="notification-time">${notification.time}</div>
                </div>
                <div class="notification-content">
                    ${notification.content}
                </div>
                ${notification.actionRequired ? this.createNotificationActions(notification) : ''}
            </div>
        `;
    }

    createNotificationActions(notification) {
        let actions = '<div class="notification-actions">';
        
        if (notification.relatedOrderId) {
            actions += `<button class="btn btn-sm btn-info" onclick="app.viewRelatedOrder('${notification.relatedOrderId}')">View Order</button>`;
        }
        
        if (!notification.read) {
            actions += `<button class="btn btn-sm btn-secondary" onclick="app.markAsRead('${notification.id}')">Mark as Read</button>`;
        }
        
        actions += '</div>';
        return actions;
    }

    createEmptyState(icon, title, description) {
        return `
            <div class="empty-state">
                <div class="empty-state-icon">${icon}</div>
                <h3 class="empty-state-title">${title}</h3>
                <p class="empty-state-description">${description}</p>
            </div>
        `;
    }

    // --- Autocomplete logic ---
    showPatientAutocomplete(type, value) {
        value = value.trim().toLowerCase();
        let matches = [];
        if (!value) {
            this.hideAutocomplete(type === 'name' ? 'patientName-autocomplete' : 'patientMRN-autocomplete');
            return;
        }
        if (type === 'name') {
            matches = this.patients.filter(p => p.name.toLowerCase().includes(value));
        } else {
            matches = this.patients.filter(p => p.mrn.toLowerCase().includes(value));
        }
        const listId = type === 'name' ? 'patientName-autocomplete' : 'patientMRN-autocomplete';
        const list = document.getElementById(listId);
        if (!list) return;
        if (matches.length === 0) {
            list.innerHTML = '';
            list.classList.remove('active');
            return;
        }
        list.innerHTML = matches.map(p => `
            <div class="autocomplete-item" data-patient='${JSON.stringify(p)}'>
                ${type === 'name' ? p.name : p.mrn} <span style="color:var(--text-muted);font-size:0.9em;">(${type === 'name' ? p.mrn : p.name})</span>
            </div>
        `).join('');
        list.classList.add('active');
        // Add click listeners
        Array.from(list.querySelectorAll('.autocomplete-item')).forEach(item => {
            item.onclick = (e) => {
                const patient = JSON.parse(item.getAttribute('data-patient'));
                this.fillPatientFields(patient);
                this.hideAutocomplete(listId);
            };
        });
    }
    hideAutocomplete(listId) {
        const list = document.getElementById(listId);
        if (list) {
            list.classList.remove('active');
            list.innerHTML = '';
        }
    }
    fillPatientFields(patient) {
        document.getElementById('patientName').value = patient.name;
        document.getElementById('patientMRN').value = patient.mrn;
        document.getElementById('patientWard').value = patient.ward;
        document.getElementById('bedNumber').value = patient.bed;
    }

    // Utility methods
    formatWardName(ward) {
        const wardNames = {
            'emergency': 'Emergency Department',
            'icu': 'Intensive Care Unit',
            'cardiology': 'Cardiology',
            'surgery': 'General Surgery',
            'pediatrics': 'Pediatrics',
            'internal': 'Internal Medicine',
            'outpatient': 'Outpatient Clinic'
        };
        return wardNames[ward] || ward;
    }

    formatStatus(status) {
        const statusNames = {
            'pending': 'Pending Review',
            'processing': 'Processing',
            'dispensing': 'Being Dispensed',
            'ready': 'Ready for Collection',
            'dispensed': 'Dispensed',
            'partially-dispensed': 'Partially Dispensed',
            'cancelled': 'Cancelled'
        };
        return statusNames[status] || status;
    }

    formatDate(dateString) {
        const date = new Date(dateString);
        return date.toLocaleDateString('en-US', { 
            year: 'numeric', 
            month: 'short', 
            day: 'numeric' 
        });
    }

    // Action methods
    async collectMedication(prescriptionId) {
        try {
            await fetch(`/api/prescriptions/${prescriptionId}/collect`, { method: 'POST', credentials: 'include' });
            await this.fetchPrescriptions();
            this.showNotification('Medication collected successfully!', 'success');
            this.refreshCurrentPage();
        } catch {
            this.showNotification('Failed to collect medication.', 'error');
        }
    }

    modifyPrescription(prescriptionId) {
        this.showNotification('Prescription modification feature coming soon!', 'info');
    }

    async cancelPrescription(prescriptionId) {
        if (confirm('Are you sure you want to cancel this prescription order?')) {
            try {
                await fetch(`/api/prescriptions/${prescriptionId}/cancel`, { method: 'POST', credentials: 'include' });
                await this.fetchPrescriptions();
                this.showNotification('Prescription cancelled.', 'warning');
                this.refreshCurrentPage();
            } catch {
                this.showNotification('Failed to cancel prescription.', 'error');
            }
        }
    }

    viewRelatedOrder(orderId) {
        const prescription = this.prescriptions.find(rx => rx.id === orderId);
        if (prescription) {
            if (['pending', 'processing', 'dispensing', 'ready'].includes(prescription.status)) {
                this.showPage('activePrescriptions');
            } else {
                this.showPage('prescriptionHistory');
            }
            
            // Highlight the related order
            setTimeout(() => {
                const orderElement = document.querySelector(`[data-order-id="${orderId}"]`);
                if (orderElement) {
                    orderElement.scrollIntoView({ behavior: 'smooth', block: 'center' });
                    orderElement.style.backgroundColor = 'rgba(59, 130, 246, 0.1)';
                    setTimeout(() => {
                        orderElement.style.backgroundColor = '';
                    }, 3000);
                }
            }, 300);
        }
    }

    async markAsRead(notificationId) {
        try {
            await fetch(`/api/notifications/${notificationId}/read`, { method: 'POST', credentials: 'include' });
            await this.fetchNotifications();
            this.updateNotificationBadges();
            this.loadNotifications();
        } catch {}
    }

    async markAllAsRead() {
        try {
            await fetch('/api/notifications/mark-all-read', { method: 'POST', credentials: 'include' });
            await this.fetchNotifications();
            this.updateNotificationBadges();
            this.loadNotifications();
            this.showNotification('All notifications marked as read.', 'success');
        } catch {}
    }

    // Notification management
    addNotification(notificationData) {
        const notification = {
            id: 'NOTIF-' + Math.random().toString(36).substr(2, 9),
            time: 'Just now',
            read: false,
            actionRequired: false,
            ...notificationData
        };
        
        this.notifications.unshift(notification);
        this.updateNotificationBadges();
        
        // If on notifications page, refresh
        if (this.currentPage === 'notifications') {
            this.loadNotifications();
        }
    }

    updateNotificationBadges() {
        const unreadCount = this.notifications.filter(n => !n.read).length;
        const badgeElements = [
            'notificationBadge',
            'notificationBadgeDesktop', 
            'sidebarNotificationBadge'
        ];
        
        badgeElements.forEach(id => {
            const element = document.getElementById(id);
            if (element) {
                element.textContent = unreadCount;
                element.style.display = unreadCount > 0 ? 'flex' : 'none';
            }
        });
    }

    // Filtering methods
    filterHistory() {
        const dateFilter = document.getElementById('historyDateFilter').value;
        const statusFilter = document.getElementById('historyStatusFilter').value;
        
        let filtered = this.prescriptions.filter(rx => 
            ['dispensed', 'partially-dispensed', 'cancelled'].includes(rx.status)
        );
        
        if (dateFilter) {
            filtered = filtered.filter(rx => rx.date === dateFilter);
        }
        
        if (statusFilter) {
            filtered = filtered.filter(rx => rx.status === statusFilter);
        }
        
        const container = document.getElementById('historyOrdersList');
        if (container) {
            if (filtered.length === 0) {
                container.innerHTML = this.createEmptyState('🔍', 'No Results Found', 'No prescriptions match your current filters.');
            } else {
                container.innerHTML = filtered.map(rx => this.createPrescriptionCard(rx, false)).join('');
            }
        }
    }

    // Utility methods
    refreshCurrentPage() {
        this.loadPageData(this.currentPage);
    }

    showNotification(message, type = 'info') {
        // Create notification element
        const notification = document.createElement('div');
        notification.className = `toast-notification toast-${type}`;
        notification.style.cssText = `
            position: fixed;
            top: 20px;
            right: 20px;
            background: var(--card-bg);
            border: 1px solid var(--border-color);
            border-radius: var(--Size-2);
            padding: var(--Size-4);
            box-shadow: 0 4px 12px var(--shadow-color);
            z-index: 10000;
            max-width: 400px;
            transform: translateX(100%);
            transition: transform 0.3s ease;
        `;
        
        const typeColors = {
            success: 'var(--success-color)',
            error: 'var(--danger-color)',
            warning: 'var(--warning-color)',
            info: 'var(--accent-color)'
        };
        
        notification.style.borderLeftColor = typeColors[type];
        notification.style.borderLeftWidth = '4px';
        
        notification.innerHTML = `
            <div style="display: flex; align-items: center; gap: var(--Size-3);">
                <span style="color: ${typeColors[type]}; font-size: var(--Size-5);">
                    ${type === 'success' ? '✅' : type === 'error' ? '❌' : type === 'warning' ? '⚠️' : 'ℹ️'}
                </span>
                <span style="color: var(--text-primary); flex: 1;">${message}</span>
                <button onclick="this.parentElement.parentElement.remove()" style="background: none; border: none; color: var(--text-secondary); cursor: pointer; font-size: var(--Size-4);">✕</button>
            </div>
        `;
        
        document.body.appendChild(notification);
        
        // Animate in
        setTimeout(() => {
            notification.style.transform = 'translateX(0)';
        }, 100);
        
        // Auto remove after 5 seconds
        setTimeout(() => {
            notification.style.transform = 'translateX(100%)';
            setTimeout(() => {
                if (notification.parentNode) {
                    notification.remove();
                }
            }, 300);
        }, 5000);
    }

    // Theme management
    toggleTheme() {
        const body = document.body;
        const currentTheme = body.getAttribute('data-theme');
        const newTheme = currentTheme === 'dark' ? 'light' : 'dark';
        
        body.setAttribute('data-theme', newTheme);
        localStorage.setItem('pharmassist-theme', newTheme);
    }
}

// Global functions for HTML onclick handlers
function showPage(pageId) {
    app.showPage(pageId);
}

function toggleMobileMenu() {
    app.toggleMobileMenu();
}

function toggleTheme() {
    app.toggleTheme();
}

function logout() {
    app.logout();
}

function markAllAsRead() {
    app.markAllAsRead();
}

function filterHistory() {
    app.filterHistory();
}

// Initialize the application
let app;

document.addEventListener('DOMContentLoaded', function() {
    app = new PharmAssistApp();
    // Set initial theme
    const savedTheme = localStorage.getItem('pharmassist-theme') || 'dark';
    document.body.setAttribute('data-theme', savedTheme);
    // Load initial data
    app.loadInitialData();
    // Handle window resize for mobile menu
    window.addEventListener('resize', function() {
        if (window.innerWidth > 768 && app.isMobileMenuOpen) {
            app.toggleMobileMenu();
        }
    });
    // Handle back button on mobile
    window.addEventListener('popstate', function() {
        if (app.isMobileMenuOpen) {
            app.toggleMobileMenu();
        }
    });
    // Add Medication logic
    const maxMedications = 3;
    const addBtn = document.getElementById('addMedicationBtn');
    const container = document.getElementById('medication-fields-container');
    let medCount = 1;
    if (addBtn && container) {
        addBtn.onclick = function() {
            if (medCount >= maxMedications) return;
            medCount++;
            const medDiv = document.createElement('div');
            medDiv.className = 'form-section medication-fields';
            medDiv.innerHTML = `
                <div class="form-row">
                    <div class="form-col">
                        <label class="form-label" for="medicationName">Medication Name *</label>
                        <select id="medicationName" name="medicationName" class="form-input" required data-medid="0">
                            <option value="">Select Medication</option>
                            <option value="Amoxicillin">Amoxicillin</option>
                            <option value="Lisinopril">Lisinopril</option>
                            <option value="Metformin">Metformin</option>
                            <option value="Atorvastatin">Atorvastatin</option>
                            <option value="Insulin Glargine">Insulin Glargine</option>
                            <option value="Epinephrine">Epinephrine</option>
                            <option value="Warfarin">Warfarin</option>
                            <option value="Paracetamol">Paracetamol</option>
                            <option value="Azithromycin">Azithromycin</option>
                        </select>
                        <div id="medication-autocomplete-0" class="autocomplete-list"></div>
                    </div>
                    <div class="form-col">
                        <label class="form-label">Strength/Concentration *</label>
                        <input type="text" name="strength" class="form-input" placeholder="e.g., 500mg, 10mg/ml" required>
                    </div>
                </div>
                <div class="form-row">
                    <div class="form-col">
                        <label class="form-label">Dosage Form *</label>
                        <select name="dosageForm" class="form-input" required>
                            <option value="">Select form</option>
                            <option value="tablet">Tablet</option>
                            <option value="capsule">Capsule</option>
                            <option value="syrup">Syrup</option>
                            <option value="injection">Injection</option>
                            <option value="cream">Topical Cream</option>
                            <option value="drops">Drops</option>
                            <option value="inhaler">Inhaler</option>
                            <option value="patch">Patch</option>
                        </select>
                    </div>
                    <div class="form-col">
                        <label class="form-label">Quantity Required *</label>
                        <input type="number" name="quantity" class="form-input" placeholder="Number of units" min="1" required>
                    </div>
                </div>
                <button type="button" class="btn btn-danger remove-med-btn" style="margin-bottom:1rem;">Remove</button>
            `;
            container.appendChild(medDiv);
            updateAddBtn();
            medDiv.querySelector('.remove-med-btn').onclick = function() {
                medDiv.remove();
                medCount--;
                updateAddBtn();
            };
        };
        function updateAddBtn() {
            addBtn.disabled = medCount >= maxMedications;
        }
    }
});

// Export for potential module usage
if (typeof module !== 'undefined' && module.exports) {
    module.exports = PharmAssistApp;
}