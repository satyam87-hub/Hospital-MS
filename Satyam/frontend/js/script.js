// Base URL for C backend CGI scripts
const BASE_URL = 'http://localhost/cgi-bin';

// Dashboard Functions
async function loadDashboardStats() {
    try {
        const doctorsCount = await fetch(`${BASE_URL}/doctor.cgi?action=count`).then(r => r.json());
        const patientsCount = await fetch(`${BASE_URL}/patient.cgi?action=count`).then(r => r.json());
        const employeesCount = await fetch(`${BASE_URL}/employee.cgi?action=count`).then(r => r.json());

        document.getElementById('totalDoctors').textContent = doctorsCount.count || 0;
        document.getElementById('totalPatients').textContent = patientsCount.count || 0;
        document.getElementById('totalEmployees').textContent = employeesCount.count || 0;
    } catch (error) {
        console.error('Error loading dashboard stats:', error);
        document.getElementById('totalDoctors').textContent = '0';
        document.getElementById('totalPatients').textContent = '0';
        document.getElementById('totalEmployees').textContent = '0';
    }
}

// Doctor Functions
async function loadDoctors() {
    try {
        const response = await fetch(`${BASE_URL}/doctor.cgi?action=list`);
        const doctors = await response.json();
        
        const tbody = document.getElementById('doctorsTableBody');
        tbody.innerHTML = '';

        if (doctors.length === 0) {
            tbody.innerHTML = '<tr><td colspan="6" class="loading">No doctors found</td></tr>';
            return;
        }

        doctors.forEach(doctor => {
            const row = `
                <tr>
                    <td>${doctor.id}</td>
                    <td>${doctor.name}</td>
                    <td>${doctor.specialization}</td>
                    <td>${doctor.phone}</td>
                    <td>${doctor.email}</td>
                    <td>
                        <button class="btn btn-edit" onclick="editDoctor(${doctor.id})">Edit</button>
                        <button class="btn btn-delete" onclick="deleteDoctor(${doctor.id})">Delete</button>
                    </td>
                </tr>
            `;
            tbody.innerHTML += row;
        });
    } catch (error) {
        console.error('Error loading doctors:', error);
        document.getElementById('doctorsTableBody').innerHTML = 
            '<tr><td colspan="6" class="loading">Error loading data</td></tr>';
    }
}

async function handleDoctorSubmit(event) {
    event.preventDefault();
    
    const id = document.getElementById('doctorId').value;
    const doctor = {
        name: document.getElementById('doctorName').value,
        specialization: document.getElementById('doctorSpecialization').value,
        phone: document.getElementById('doctorPhone').value,
        email: document.getElementById('doctorEmail').value
    };

    try {
        const action = id ? 'update' : 'add';
        if (id) doctor.id = id;

        const response = await fetch(`${BASE_URL}/doctor.cgi`, {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({action, ...doctor})
        });

        const result = await response.json();
        if (result.success) {
            closeModal('doctor');
            loadDoctors();
            alert(id ? 'Doctor updated successfully!' : 'Doctor added successfully!');
        } else {
            alert('Error: ' + result.message);
        }
    } catch (error) {
        console.error('Error saving doctor:', error);
        alert('Error saving doctor data');
    }
}

async function editDoctor(id) {
    try {
        const response = await fetch(`${BASE_URL}/doctor.cgi?action=get&id=${id}`);
        const doctor = await response.json();

        document.getElementById('doctorId').value = doctor.id;
        document.getElementById('doctorName').value = doctor.name;
        document.getElementById('doctorSpecialization').value = doctor.specialization;
        document.getElementById('doctorPhone').value = doctor.phone;
        document.getElementById('doctorEmail').value = doctor.email;

        document.getElementById('doctorModalTitle').textContent = 'Edit Doctor';
        document.getElementById('doctorModal').style.display = 'block';
    } catch (error) {
        console.error('Error loading doctor:', error);
        alert('Error loading doctor data');
    }
}

async function deleteDoctor(id) {
    if (!confirm('Are you sure you want to delete this doctor?')) return;

    try {
        const response = await fetch(`${BASE_URL}/doctor.cgi`, {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({action: 'delete', id})
        });

        const result = await response.json();
        if (result.success) {
            loadDoctors();
            alert('Doctor deleted successfully!');
        } else {
            alert('Error: ' + result.message);
        }
    } catch (error) {
        console.error('Error deleting doctor:', error);
        alert('Error deleting doctor');
    }
}

// Patient Functions
async function loadPatients() {
    try {
        const response = await fetch(`${BASE_URL}/patient.cgi?action=list`);
        const patients = await response.json();
        
        const tbody = document.getElementById('patientsTableBody');
        tbody.innerHTML = '';

        if (patients.length === 0) {
            tbody.innerHTML = '<tr><td colspan="8" class="loading">No patients found</td></tr>';
            return;
        }

        patients.forEach(patient => {
            const row = `
                <tr>
                    <td>${patient.id}</td>
                    <td>${patient.name}</td>
                    <td>${patient.age}</td>
                    <td>${patient.gender}</td>
                    <td>${patient.phone}</td>
                    <td>${patient.disease}</td>
                    <td>${patient.admission_date}</td>
                    <td>
                        <button class="btn btn-edit" onclick="editPatient(${patient.id})">Edit</button>
                        <button class="btn btn-delete" onclick="deletePatient(${patient.id})">Delete</button>
                    </td>
                </tr>
            `;
            tbody.innerHTML += row;
        });
    } catch (error) {
        console.error('Error loading patients:', error);
        document.getElementById('patientsTableBody').innerHTML = 
            '<tr><td colspan="8" class="loading">Error loading data</td></tr>';
    }
}

async function handlePatientSubmit(event) {
    event.preventDefault();
    
    const id = document.getElementById('patientId').value;
    const patient = {
        name: document.getElementById('patientName').value,
        age: document.getElementById('patientAge').value,
        gender: document.getElementById('patientGender').value,
        phone: document.getElementById('patientPhone').value,
        disease: document.getElementById('patientDisease').value,
        admission_date: document.getElementById('patientAdmissionDate').value
    };

    try {
        const action = id ? 'update' : 'add';
        if (id) patient.id = id;

        const response = await fetch(`${BASE_URL}/patient.cgi`, {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({action, ...patient})
        });

        const result = await response.json();
        if (result.success) {
            closeModal('patient');
            loadPatients();
            alert(id ? 'Patient updated successfully!' : 'Patient added successfully!');
        } else {
            alert('Error: ' + result.message);
        }
    } catch (error) {
        console.error('Error saving patient:', error);
        alert('Error saving patient data');
    }
}

async function editPatient(id) {
    try {
        const response = await fetch(`${BASE_URL}/patient.cgi?action=get&id=${id}`);
        const patient = await response.json();

        document.getElementById('patientId').value = patient.id;
        document.getElementById('patientName').value = patient.name;
        document.getElementById('patientAge').value = patient.age;
        document.getElementById('patientGender').value = patient.gender;
        document.getElementById('patientPhone').value = patient.phone;
        document.getElementById('patientDisease').value = patient.disease;
        document.getElementById('patientAdmissionDate').value = patient.admission_date;

        document.getElementById('patientModalTitle').textContent = 'Edit Patient';
        document.getElementById('patientModal').style.display = 'block';
    } catch (error) {
        console.error('Error loading patient:', error);
        alert('Error loading patient data');
    }
}

async function deletePatient(id) {
    if (!confirm('Are you sure you want to delete this patient?')) return;

    try {
        const response = await fetch(`${BASE_URL}/patient.cgi`, {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({action: 'delete', id})
        });

        const result = await response.json();
        if (result.success) {
            loadPatients();
            alert('Patient deleted successfully!');
        } else {
            alert('Error: ' + result.message);
        }
    } catch (error) {
        console.error('Error deleting patient:', error);
        alert('Error deleting patient');
    }
}

// Employee Functions
async function loadEmployees() {
    try {
        const response = await fetch(`${BASE_URL}/employee.cgi?action=list`);
        const employees = await response.json();
        
        const tbody = document.getElementById('employeesTableBody');
        tbody.innerHTML = '';

        if (employees.length === 0) {
            tbody.innerHTML = '<tr><td colspan="7" class="loading">No employees found</td></tr>';
            return;
        }

        employees.forEach(employee => {
            const row = `
                <tr>
                    <td>${employee.id}</td>
                    <td>${employee.name}</td>
                    <td>${employee.position}</td>
                    <td>${employee.department}</td>
                    <td>${employee.phone}</td>
                    <td>${employee.email}</td>
                    <td>
                        <button class="btn btn-edit" onclick="editEmployee(${employee.id})">Edit</button>
                        <button class="btn btn-delete" onclick="deleteEmployee(${employee.id})">Delete</button>
                    </td>
                </tr>
            `;
            tbody.innerHTML += row;
        });
    } catch (error) {
        console.error('Error loading employees:', error);
        document.getElementById('employeesTableBody').innerHTML = 
            '<tr><td colspan="7" class="loading">Error loading data</td></tr>';
    }
}

async function handleEmployeeSubmit(event) {
    event.preventDefault();
    
    const id = document.getElementById('employeeId').value;
    const employee = {
        name: document.getElementById('employeeName').value,
        position: document.getElementById('employeePosition').value,
        department: document.getElementById('employeeDepartment').value,
        phone: document.getElementById('employeePhone').value,
        email: document.getElementById('employeeEmail').value
    };

    try {
        const action = id ? 'update' : 'add';
        if (id) employee.id = id;

        const response = await fetch(`${BASE_URL}/employee.cgi`, {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({action, ...employee})
        });

        const result = await response.json();
        if (result.success) {
            closeModal('employee');
            loadEmployees();
            alert(id ? 'Employee updated successfully!' : 'Employee added successfully!');
        } else {
            alert('Error: ' + result.message);
        }
    } catch (error) {
        console.error('Error saving employee:', error);
        alert('Error saving employee data');
    }
}

async function editEmployee(id) {
    try {
        const response = await fetch(`${BASE_URL}/employee.cgi?action=get&id=${id}`);
        const employee = await response.json();

        document.getElementById('employeeId').value = employee.id;
        document.getElementById('employeeName').value = employee.name;
        document.getElementById('employeePosition').value = employee.position;
        document.getElementById('employeeDepartment').value = employee.department;
        document.getElementById('employeePhone').value = employee.phone;
        document.getElementById('employeeEmail').value = employee.email;

        document.getElementById('employeeModalTitle').textContent = 'Edit Employee';
        document.getElementById('employeeModal').style.display = 'block';
    } catch (error) {
        console.error('Error loading employee:', error);
        alert('Error loading employee data');
    }
}

async function deleteEmployee(id) {
    if (!confirm('Are you sure you want to delete this employee?')) return;

    try {
        const response = await fetch(`${BASE_URL}/employee.cgi`, {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({action: 'delete', id})
        });

        const result = await response.json();
        if (result.success) {
            loadEmployees();
            alert('Employee deleted successfully!');
        } else {
            alert('Error: ' + result.message);
        }
    } catch (error) {
        console.error('Error deleting employee:', error);
        alert('Error deleting employee');
    }
}

// Modal Functions
function showAddModal(type) {
    document.getElementById(`${type}Form`).reset();
    document.getElementById(`${type}Id`).value = '';
    document.getElementById(`${type}ModalTitle`).textContent = `Add ${type.charAt(0).toUpperCase() + type.slice(1)}`;
    document.getElementById(`${type}Modal`).style.display = 'block';
}

function closeModal(type) {
    document.getElementById(`${type}Modal`).style.display = 'none';
}

// Search Function
function searchTable(tableId, searchValue) {
    const tbody = document.getElementById(tableId);
    const rows = tbody.getElementsByTagName('tr');

    for (let row of rows) {
        const text = row.textContent.toLowerCase();
        if (text.includes(searchValue.toLowerCase())) {
            row.style.display = '';
        } else {
            row.style.display = 'none';
        }
    }
}

// Close modal when clicking outside
window.onclick = function(event) {
    if (event.target.className === 'modal') {
        event.target.style.display = 'none';
    }
}
