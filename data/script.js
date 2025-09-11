// Application State Management
class PharmAssistApp {
    constructor() {
        this.currentUser = null;
        this.prescriptions = [];
        this.notifications = [];
        this.currentPage = 'prescriptionOrder';
        this.isMobileMenuOpen = false;
        
        this.initializeData();
        this.initializeEventListeners();
    }

    // Initialize sample data
    initializeData() {
        // Sample prescription orders
        this.prescriptions = [
            {
                id: 'RX-2024-001',
                patientName: 'Sarah Johnson',
                patientMRN: 'MRN-78901234',
                medicationName: 'Lisinopril',
                strength: '10mg',
                dosageForm: 'tablet',
                quantity: 30,
                status: 'dispensing',
                priority: 'routine',
                date: '2024-01-16',
                ward: 'cardiology',
                route: 'oral',
                frequency: 'once',
                indication: 'Hypertension management',
                prescribingPhysician: 'Dr. Smith'
            },
            {
                id: 'RX-2024-002',
                patientName: 'Michael Rodriguez',
                patientMRN: 'MRN-56789012',
                medicationName: 'Insulin Glargine',
                strength: '100IU/ml',
                dosageForm: 'injection',
                quantity: 3,
                status: 'pending',
                priority: 'urgent',
                date: '2024-01-16',
                ward: 'internal',
                route: 'subcutaneous',
                frequency: 'once',
                indication: 'Diabetes mellitus type 1',
                prescribingPhysician: 'Dr. Smith'
            },
            {
                id: 'RX-2024-003',
                patientName: 'Emma Thompson',
                patientMRN: 'MRN-34567890',
                medicationName: 'Epinephrine',
                strength: '1mg/ml',
                dosageForm: 'injection',
                quantity: 1,
                status: 'ready',
                priority: 'stat',
                date: '2024-01-16',
                ward: 'emergency',
                route: 'intramuscular',
                frequency: 'stat',
                indication: 'Anaphylactic reaction',
                prescribingPhysician: 'Dr. Smith'
            },
            {
                id: 'RX-2024-004',
                patientName: 'Robert Chen',
                patientMRN: 'MRN-23456789',
                medicationName: 'Amoxicillin',
                strength: '500mg',
                dosageForm: 'capsule',
                quantity: 21,
                status: 'dispensed',
                priority: 'routine',
                date: '2024-01-15',
                ward: 'outpatient',
                route: 'oral',
                frequency: 'tid',
                indication: 'Bacterial pneumonia',
                prescribingPhysician: 'Dr. Smith',
                completedDate: '2024-01-15',
                dispensedBy: 'Pharmacist Jones'
            },
            {
                id: 'RX-2024-005',
                patientName: 'Lisa Williams',
                patientMRN: 'MRN-12345678',
                medicationName: 'Warfarin',
                strength: '5mg',
                dosageForm: 'tablet',
                quantity: 30,
                status: 'partially-dispensed',
                priority: 'routine',
                date: '2024-01-14',
                ward: 'cardiology',
                route: 'oral',
                frequency: 'once',
                indication: 'Atrial fibrillation',
                prescribingPhysician: 'Dr. Smith',
                partialQuantity: 15,
                partialReason: 'Limited stock available'
            }
        ];

        // Sample notifications
        this.notifications = [
            {
                id: 'NOTIF-001',
                title: 'Epinephrine Available',
                content: 'Emergency medication for Patient Emma Thompson (RX-2024-003) is now ready for collection from pharmacy.',
                type: 'success',
                priority: 'urgent',
                time: '5 minutes ago',
                read: false,
                actionRequired: true,
                relatedOrderId: 'RX-2024-003'
            },
            {
                id: 'NOTIF-002',
                title: 'Stock Alert: Insulin Glargine',
                content: 'Limited stock remaining. Your prescription RX-2024-002 may experience delays. Alternative formulation available.',
                type: 'urgent',
                priority: 'high',
                time: '15 minutes ago',
                read: false,
                actionRequired: true,
                relatedOrderId: 'RX-2024-002'
            },
            {
                id: 'NOTIF-003',
                title: 'Prescription Dispensed',
                content: 'Amoxicillin 500mg for Robert Chen has been successfully dispensed. Patient notified for collection.',
                type: 'success',
                priority: 'normal',
                time: '2 hours ago',
                read: true,
                actionRequired: false,
                relatedOrderId: 'RX-2024-004'
            },
            {
                id: 'NOTIF-004',
                title: 'Partial Dispensing Notice',
                content: 'Warfarin 5mg for Lisa Williams partially dispensed (15/30 tablets). Remaining stock expected tomorrow.',
                type: 'unread',
                priority: 'normal',
                time: '1 day ago',
                read: false,
                actionRequired: false,
                relatedOrderId: 'RX-2024-005'
            }
        ];

        this.updateNotificationBadges();
    }

    // Initialize event listeners
    initializeEventListeners() {
        // Login form
        const loginForm = document.getElementById('loginForm');
        if (loginForm) {
            loginForm.addEventListener('submit', (e) => this.handleLogin(e));
        }

        // Prescription form
        const prescriptionForm = document.getElementById('prescriptionForm');
        if (prescriptionForm) {
            prescriptionForm.addEventListener('submit', (e) => this.handlePrescriptionSubmit(e));
        }

        // Theme toggle
        document.addEventListener('DOMContentLoaded', () => {
            const savedTheme = localStorage.getItem('pharmassist-theme') || 'dark';
            document.body.setAttribute('data-theme', savedTheme);
        });
    }

    // Authentication methods
    handleLogin(e) {
        e.preventDefault();
        const email = document.getElementById('email').value;
        const password = document.getElementById('password').value;
        
        if (email && password) {
            // Simulate authentication
            this.currentUser = {
                name: this.extractDoctorName(email),
                email: email,
                license: 'MD-' + Math.random().toString().substr(2, 9)
            };

            this.showDashboard();
            this.loadInitialData();
        }
    }

    extractDoctorName(email) {
        const name = email.split('@')[0];
        return name.charAt(0).toUpperCase() + name.slice(1);
    }

    showDashboard() {
        document.getElementById('loginPage').classList.add('hidden');
        document.getElementById('dashboard').classList.remove('hidden');
        
        // Update user info in multiple places
        const elements = ['physicianName', 'sidebarPhysicianName'];
        elements.forEach(id => {
            const element = document.getElementById(id);
            if (element) {
                element.textContent = this.currentUser.name;
            }
        });
    }

    logout() {
        this.currentUser = null;
        document.getElementById('dashboard').classList.add('hidden');
        document.getElementById('loginPage').classList.remove('hidden');
        document.getElementById('loginForm').reset();
        
        // Close mobile menu if open
        if (this.isMobileMenuOpen) {
            this.toggleMobileMenu();
        }
    }

    // Navigation methods
    showPage(pageId) {
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
    handlePrescriptionSubmit(e) {
        e.preventDefault();
        
        const formData = new FormData(e.target);
        const prescriptionData = {
            id: this.generatePrescriptionId(),
            patientName: document.getElementById('patientName').value,
            patientMRN: document.getElementById('patientMRN').value,
            medicationName: document.getElementById('medicationName').value,
            strength: document.getElementById('strength').value,
            dosageForm: document.getElementById('dosageForm').value,
            quantity: parseInt(document.getElementById('quantity').value),
            route: document.getElementById('route').value,
            frequency: document.getElementById('frequency').value,
            priority: document.getElementById('priority').value,
            indication: document.getElementById('indication').value,
            specialInstructions: document.getElementById('specialInstructions').value,
            ward: document.getElementById('patientWard').value,
            bedNumber: document.getElementById('bedNumber').value,
            status: 'pending',
            date: new Date().toISOString().split('T')[0],
            prescribingPhysician: `Dr. ${this.currentUser.name}`
        };
        
        this.prescriptions.unshift(prescriptionData);
        
        // Show success notification
        this.showNotification('Prescription submitted successfully!', 'success');
        
        // Reset form
        e.target.reset();
        
        // Add to notifications
        this.addNotification({
            title: 'Prescription Submitted',
            content: `New prescription for ${prescriptionData.patientName} - ${prescriptionData.medicationName} has been submitted to pharmacy.`,
            type: 'success',
            priority: prescriptionData.priority === 'stat' ? 'urgent' : 'normal',
            relatedOrderId: prescriptionData.id
        });
        
        // Reload active prescriptions if on that page
        if (this.currentPage === 'activePrescriptions') {
            this.loadActivePrescriptions();
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
    collectMedication(prescriptionId) {
        const prescription = this.prescriptions.find(rx => rx.id === prescriptionId);
        if (prescription) {
            prescription.status = 'dispensed';
            prescription.completedDate = new Date().toISOString().split('T')[0];
            prescription.dispensedBy = 'Pharmacy Staff';
            
            this.showNotification(`${prescription.medicationName} collected successfully!`, 'success');
            
            // Add notification
            this.addNotification({
                title: 'Medication Collected',
                content: `${prescription.medicationName} for ${prescription.patientName} has been collected.`,
                type: 'success',
                priority: 'normal',
                relatedOrderId: prescriptionId
            });
            
            this.refreshCurrentPage();
        }
    }

    modifyPrescription(prescriptionId) {
        this.showNotification('Prescription modification feature coming soon!', 'info');
    }

    cancelPrescription(prescriptionId) {
        if (confirm('Are you sure you want to cancel this prescription order?')) {
            const prescription = this.prescriptions.find(rx => rx.id === prescriptionId);
            if (prescription) {
                prescription.status = 'cancelled';
                prescription.completedDate = new Date().toISOString().split('T')[0];
                
                this.showNotification(`Prescription ${prescriptionId} cancelled.`, 'warning');
                
                // Add notification
                this.addNotification({
                    title: 'Prescription Cancelled',
                    content: `Prescription for ${prescription.patientName} - ${prescription.medicationName} has been cancelled.`,
                    type: 'unread',
                    priority: 'normal',
                    relatedOrderId: prescriptionId
                });
                
                this.refreshCurrentPage();
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

    markAsRead(notificationId) {
        const notification = this.notifications.find(n => n.id === notificationId);
        if (notification) {
            notification.read = true;
            this.updateNotificationBadges();
            this.loadNotifications();
        }
    }

    markAllAsRead() {
        this.notifications.forEach(notification => {
            notification.read = true;
        });
        this.updateNotificationBadges();
        this.loadNotifications();
        this.showNotification('All notifications marked as read.', 'success');
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
});

// Export for potential module usage
if (typeof module !== 'undefined' && module.exports) {
    module.exports = PharmAssistApp;
}