
<div class="container mt-4">
    <div class="card shadow mb-4">
        <div class="card-header py-3">
            <h6 class="m-0 font-weight-bold" style="color: #11101d">Liste Profil</h6>
        </div>
        <div class="card-body">
            <div class="container mt-4">
                <table class="table table-bordered" id="profilList">
                    <thead style="text-align: center;">
                        <tr >
                            <th style="display: none;">Id </th>
                            <th>Code Profile</th>
                            <th>libelle</th>
                            <th>Action</th>
                        </tr>
                    </thead>
                    <tbody style="text-align: center;">
                        <?php foreach ($data as $profile) { ?>
                            <tr>
                                <td style="display: none;"><?php echo $profile->id; ?></td>
                                <td><?php echo  $profile->code_profile; ?></td>
                                <td><?php  echo $profile->libelle; ?></td>
                                <td style="text-align: center ;">
                                    <form action="" method="DELETE">
                                        <a class="btn btn-info" href="#" onclick="modifier('<?php echo $profile->id; ?>', '<?php echo $profile->code_profile; ?>', '<?php echo $profile->libelle; ?>')"><i class="fa-regular fa-pen-to-square" style="margin-right: 5px;"></i>Modifier</a>
                                        <button class="btn btn-danger" onclick="supprimer('<?php echo $profile->id; ?>')"><i class="fa-solid fa-trash" style="margin-right: 5px;"></i>Supprimer</button>
                                    </form>
                                </td>
                            </tr>
                        <?php }; ?>
                    </tbody>
                </table>
            </div>
        </div>
    </div>
</div>
    




<script type="text/javascript">
    function modifier(id, code_profile, libelle) {
        document.getElementById("code_profile").value= code_profile;
        document.getElementById("code_profile").disabled= true;
        document.getElementById("libelle").value= libelle;
        document.getElementById("id").value = id;
    }

    function supprimer(id) {
        var confirm= window.confirm("Voulez-vous vraiment supprimer");

        if (confirm) {
            $.ajax({
                url: "<?php echo site_url("profiles/remove") ?>",
                type: "POST",
                data: {
                    "id": id
                },
                success: function (response) {
                    window.location.reload();
                },
                error: function (error) {
                    alert(error);
                }
            });
        }
       
    }


   $("#addForm").submit(function(e) {

    e.preventDefault();
    var code_profile= $("#code_profile").val();
    var libelle= $("#libelle").val();
    var id = $("#id").val();

    if(id) {
        $.ajax({
                url: "<?php echo site_url("profiles/edit") ?>",
                type: "PUT",
                data: JSON.stringify({
                    id: id,
                    code_profile: code_profile,
                    libelle: libelle
                }),
                success: function(response) {
                    window.location.reload();
            
                },
                error: function(error) {
                    console.log(error);
                }
            });
        
    }else {
        $.ajax({
                url: "<?php echo site_url("profiles/create") ?>",
                type: "POST",
                data: {
                    "code_profile": code_profile,
                    "libelle": libelle
                },
                success: function(response) {
                    window.location.reload();
                },
                error: function(error) {
                    console.log(error);
                }
            });
    }
   });

   $(document).ready(function() {
        $('#profilList').DataTable( {
        language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
        }
        });
    });
</script






    
