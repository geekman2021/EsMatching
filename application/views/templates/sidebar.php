<!DOCTYPE html>
<!-- Coding by CodingNepal | www.codingnepalweb.com -->
<html lang="en" dir="ltr">
  <head>
    <meta charset="UTF-8" />
    <title>Rapprochement</title>

    <script src="<?php echo base_url(); ?>assets/jquery/jquery-3.7.1.min.js"></script>
    <script src="<?php  echo base_url(); ?>assets/jquery/jquery.dataTables.min.js"></script>
     <script src="<?php echo base_url(); ?>assets/moment.min.js"></script>
     <script src="https://cdnjs.cloudflare.com/ajax/libs/tempusdominus-bootstrap-4/5.39.0/js/tempusdominus-bootstrap-4.min.js"></script>

<!-- Inclure le fichier de langue française -->
<script src="https://cdnjs.cloudflare.com/ajax/libs/moment.js/2.29.1/locale/fr.js"></script>
     <!-- <script src="https://cdnjs.cloudflare.com/ajax/libs/tempusdominus-bootstrap-4/5.1.2/js/tempusdominus-bootstrap-4.min.js"></script>

     <script src="https://cdnjs.cloudflare.com/ajax/libs/moment.js/2.29.1/locale/fr.js"></script> -->
     <script src="<?php echo base_url(); ?>assets/jquery/dataTables.dateTime.min.js"></script>
     <script src="<?php echo base_url(); ?>assets/jquery/dataTables.buttons.min.js"></script>
    <!-- <script src="<?php echo base_url(); ?>assets/jquery/jszip.min.js"></script>
    <script src="<?php echo base_url(); ?>assets/jquery/pdfmake.min.js"></script>
    <script src="<?php echo base_url(); ?>assets/jquery/vfs_fonts.js"></script>
    <script src="<?php echo base_url(); ?>assets/jquery/buttons.html5.min.js"></script>
    <script src="<?php echo base_url(); ?>assets/jquery/vfs_fonts.js"></script>
    <script src="<?php echo base_url(); ?>assets/jquery/buttons.html5.min.js"></script>
    <script src="<?php echo base_url(); ?>assets/jquery/buttons.print.min.js"></script>-->
    
    <!-- <script  src="<?php echo base_url(); ?>assets/bootstrap/js/popper.js"></script> -->
    <script src="<?php echo base_url(); ?>assets/sweetalert.min.js"></script> 
    

    <script src="<?php echo base_url(); ?>assets/bootstrap/js/bootstrap.min.js"></script>

    <script src="<?php echo base_url(); ?>assets/chart/chart.js"> </script>
    <link rel="stylesheet" href="<?php echo base_url(); ?>assets/bootstrap/css/bootstrap.css" />
    <!-- <script src="https://cdn.jsdelivr.net/npm/jquery@3.6.4/dist/jquery.slim.min.js"></script> -->
    <!-- Inclure le fichier CSS Bootstrap -->
<!-- <link href="https://cdn.jsdelivr.net/npm/bootstrap@4.5.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-pzjw8f+ua7Kw1TIq0v8FqFjcJ6pajs/rfdfs3SO+k5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5G5r5 -->

 
    <link rel="stylesheet" href="<?php  echo base_url(); ?>assets/jquery/jquery.dataTables.min.css" />
    <link rel="stylesheet" type="text/css" href="<?php echo base_url(); ?>assets/dataTables.dateTime.min.css" >
    <script src="<?php echo base_url(); ?>assets/jquery/datatables.min.js"> </script>
    <link rel="stylesheet" href="<?php echo base_url(); ?>assets/jquery/datatables.css" />
    <link rel="stylesheet" type="text/css" href="<?php echo base_url(); ?>assets/jquery/buttons.dataTables.min.css">
    <link rel="stylesheet" type="text/css" href="<?php echo base_url(); ?>assets/font-awesome/css/all.css" >
    <link rel="stylesheet" type="text/css" href="<?php echo base_url(); ?>assets/font-awesome/css/fontawesome.min.css" >
    <script src="<?php echo base_url(); ?>assets/jquery/jquery.dataTables.min.js"></script>
    <link href="https://unpkg.com/boxicons@2.0.7/css/boxicons.min.css" rel="stylesheet" />
    <link rel="stylesheet" href="<?php echo base_url();?>assets/style.css">
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />

  </head>
  <body>
    <div class="sidebar close">
      <div class="logo-details">
        <i class="bx bxl-c-plus-plus"></i>
        <span class="logo_name">Monétique</span>
      </div>
      <ul class="nav-links">
        <li>
          <a href="<?php echo site_url("dashboard") ?>">
            <i class="bx bx-grid-alt"></i>
            <span class="link_name">Dashboard</span>
          </a>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("dashboard") ?>">Dashboard</a></li>
          </ul>
        </li>
        <li>
          <div class="iocn-link">
            <a href="<?php echo site_url("users") ?>">
              <i class="bx bx-collection"></i>
              <span class="link_name">Utilisateur</span>
            </a>
          </div>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("users") ?>">Utilisateur</a></li>
          </ul>
        </li>
        <li>
          <div class="iocn-link">
            <a href="<?php echo site_url("profiles") ?>">
              <i class="bx bx-book-alt"></i>
              <span class="link_name">Profil</span>
            </a>
          </div>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("profiles") ?>">Profil</a></li>
          </ul>
        </li>
        <li>
          <div class="iocn-link">
            <a href="<?php echo site_url("droits") ?>">
              <i class="bx bx-book-alt"></i>
              <span class="link_name">Droit Utlisateur</span>
            </a>
          </div>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("droits") ?>">Droit Utilisateur</a></li>
          </ul>
        </li>
        <li>
          <a href="<?php echo site_url("importer") ?>">
            <i class="bx bx-compass"></i>
            <span class="link_name">Importer</span>
          </a>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("importer") ?>">Importer</a></li>
          </ul>
        </li>
        <li>
          <div class="iocn-link">
            <a href="#">
              <i class="bx bx-plug"></i>
              <span class="link_name">Operateur</span>
            </a>
            <i class="bx bxs-chevron-down arrow"></i>
          </div>
          <ul class="sub-menu">
            <li><a class="link_name" href="#">Operateur</a></li>
            <li><a href="<?php echo site_url("airtel") ?>">Airtel</a></li>
            <li><a href="<?php echo site_url("orange") ?>">Orange</a></li>
            <li><a href="<?php echo site_url("telma") ?>">Telma</a></li>
          </ul>
        </li>

        <li>
          <a href="<?php echo site_url("regul") ?>">
            <i class="bx bx-compass"></i>
            <span class="link_name">Regularisation</span>
          </a>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("regul") ?>">Regularisation</a></li>
          </ul>
        </li>
        <li>
          <a href="<?php echo site_url("historique") ?>">
            <i class="bx bx-history"></i>
            <span class="link_name">Historique</span>
          </a>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("historique") ?>">Historique</a></li>
          </ul>
        </li>
        <li>
          <!-- <div class="profile-details">
            <div class="profile-content">
              <img src="image/profile.jpg" alt="profileImg" />
            </div>
            <div class="name-job">
              <div class="profile_name">Prem Shahi</div>
              <div class="job">Web Desginer</div>
            </div>
            <i class="bx bx-log-out"></i>
          </div> -->
        </li>
      </ul>
    </div>
    <section class="home-section">
      <div class="home-content">
        <div class="row" style="display: flex; justify-content: space-between; align-items: center;">
          <div>
            <i class="bx bx-menu"></i>
          </div>
          <div style="position: absolute; right: 0px;">
          <a class="nav-link dropdown-toggle" href="#" id="userDropdown" role="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
            <img class="img-profile rounded-circle" src="<?php echo base_url(); ?>assets/images/undraw_profile.svg" style="width: 50px; height: 50px;">
          </a>
          </div>
        </div>
      </div>

    
  
</html>
<script>
      let arrow = document.querySelectorAll(".arrow");
      for (var i = 0; i < arrow.length; i++) {
        arrow[i].addEventListener("click", (e) => {
          let arrowParent = e.target.parentElement.parentElement; //selecting main parent of arrow
          arrowParent.classList.toggle("showMenu");
        });
      }
      let sidebar = document.querySelector(".sidebar");
      let sidebarBtn = document.querySelector(".bx-menu");
      console.log(sidebarBtn);
      sidebarBtn.addEventListener("click", () => {
        sidebar.classList.toggle("close");
      });
</script>

